//
// Created by maarten on 11-08-20.
//

#include "vtkMyCityJSONReader.h"

// VTK Includes
#include "vtkAbstractArray.h"
#include "vtkBitArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkMyCityJSONFeature.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkStringArray.h"
#include "vtkTriangleFilter.h"
#include "vtk_jsoncpp.h"
#include "vtksys/FStream.hxx"

// C++ includes
#include <iostream>


vtkStandardNewMacro(vtkMyCityJSONReader);

//----------------------------------------------------------------------------
class vtkMyCityJSONReader::CityJSONReaderInternal {
public:
    typedef struct {
        std::string Name;
        vtkVariant Value;
    } CityJSONProperty;

    // List of property names to read. Property value is used the default
    std::vector<CityJSONProperty> PropertySpecs;

    // Parse the Json Value corresponding to the root of the CityJSON data from the file
    void ParseRoot(const Json::Value &root, vtkPolyData *output, bool outlinePolygons,
                   const char *serializedPropertiesArrayName);

    // Verify if file exists and can be read by the parser
    // If exists, parse into Jsoncpp data structure
    static int CanParseFile(const char *filename, Json::Value &root);

    // Verify if string can be read by the parser
    // If exists, parse into Jsoncpp data structure
    static int CanParseString(char *input, Json::Value &root);

};

//----------------------------------------------------------------------------
void vtkMyCityJSONReader::CityJSONReaderInternal::ParseRoot(const Json::Value &root, vtkPolyData *output, bool outlinePolygons, const char *serializedPropertiesArrayName) {
    // Initialize geometry containers
    vtkNew<vtkPoints> points;
    points->SetDataTypeToDouble();
    output->SetPoints(points);

    vtkNew<vtkCellArray> verts;
    output->SetVerts(verts);

    vtkNew<vtkCellArray> lines;
    output->SetLines(lines);

    vtkNew<vtkCellArray> polys;
    output->SetPolys(polys);

    // Initialize vertex-id array
    vtkIntArray *vertexIdArray = vtkIntArray::New();
    vertexIdArray->SetName("vertex-id");
    output->GetCellData()->AddArray(vertexIdArray);
    vertexIdArray->Delete();

    // Initialize properties arrays
    if (serializedPropertiesArrayName) {
        vtkStringArray *propertiesArray = vtkStringArray::New();
        propertiesArray->SetName(serializedPropertiesArrayName);
        output->GetCellData()->AddArray(propertiesArray);
        propertiesArray->Delete();
    }

    vtkAbstractArray *array;
    auto iter = this->PropertySpecs.begin();
    for (; iter != this->PropertySpecs.end(); ++iter) {
        array = nullptr;
        switch (iter->Value.GetType()) {
            case VTK_BIT:
                array = vtkBitArray::New();
                break;

            case VTK_INT:
                array = vtkIntArray::New();
                break;

            case VTK_DOUBLE:
                array = vtkDoubleArray::New();
                break;

            case VTK_STRING:
                array = vtkStringArray::New();
                break;

            default:
                vtkGenericWarningMacro("unexpected data type " << iter->Value.GetType());
                break;
        }

        // Skip if array not created for some reason
        if (!array) {
            continue;
        }

        array->SetName(iter->Name.c_str());
        output->GetCellData()->AddArray(array);
        array->Delete();
    }

    // Check type
    Json::Value rootType = root["type"];
    if (rootType.isNull()) {
        vtkGenericWarningMacro(<< "ParseRoot: Missing type node");
        return;
    } else if (rootType.asString() != "CityJSON") {
        vtkGenericWarningMacro(<< "ParseRoot: \"type\" is not \"CityJSON\"");
    }

    vtkNew<vtkMyCityJSONFeature> feature;

    // Parse vertices
    Json::Value rootVertices = root["vertices"];
    if (!rootVertices.isArray()) {
        vtkGenericWarningMacro(<< "ParseRoot: \"vertices\" not of type \"array\"");
    } else {
        feature->ExtractVertices(rootVertices, output);

    }

    // Parse polygons
    Json::Value rootCityObjects = root["CityObjects"];
    if (rootCityObjects.isNull()) {
        vtkGenericWarningMacro(<< "ParseRoot: Missing \"CityObjects\" node");
    } else {
        for (Json::Value cityObject : rootCityObjects) {  // GUID_Something
            feature->ConnectTheDots(cityObject, output);
        }
    }
}


//----------------------------------------------------------------------------
int vtkMyCityJSONReader::CityJSONReaderInternal::CanParseFile(const char *filename, Json::Value &root) {
    if (!filename) {
        vtkGenericWarningMacro(<< "Input filename not specified");
        return VTK_ERROR;
    }

    vtksys::ifstream file;
    file.open(filename);

    if (!file.is_open()) {
        vtkGenericWarningMacro(<< "Unable to Open File " << filename);
        return VTK_ERROR;
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    std::string formattedErrors;

    // parse the entire cityJSON data into the Json::Value root
    bool parsedSuccess = parseFromStream(builder, file, &root, &formattedErrors);

    if (!parsedSuccess) {
        // Report failures and their locations in the document
        vtkGenericWarningMacro(<< "Failed to parse JSON" << endl << formattedErrors);
        return VTK_ERROR;
    }

    return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkMyCityJSONReader::CityJSONReaderInternal::CanParseString(char *input, Json::Value &root) {
    if (!input) {
        vtkGenericWarningMacro(<< "Input string is empty");
        return VTK_ERROR;
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    std::string formattedErrors;

    // parse the entire cityJSON data into the Json::Value root
    bool parsedSuccess = reader->parse(input, input + strlen(input), &root, &formattedErrors);

    if (!parsedSuccess) {
        // Report failures and their locations in the document
        vtkGenericWarningMacro(<< "Failed to parse JSON" << endl << formattedErrors);
        return VTK_ERROR;
    }

    return VTK_OK;
}

//----------------------------------------------------------------------------
vtkMyCityJSONReader::vtkMyCityJSONReader() {
    this->FileName = nullptr;
    this->StringInput = nullptr;
    this->StringInputMode = false;
    this->TriangulatePolygons = false;
    this->OutlinePolygons = false;
    this->SerializedPropertiesArrayName = nullptr;
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
    this->Internal = new CityJSONReaderInternal;
}

//----------------------------------------------------------------------------
vtkMyCityJSONReader::~vtkMyCityJSONReader() {
    delete[] FileName;
    delete Internal;
}

//----------------------------------------------------------------------------
int vtkMyCityJSONReader::RequestData(vtkInformation * vtkNotUsed(request), vtkInformationVector ** vtkNotUsed(request), vtkInformationVector *outputVector) {
    // Get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // Get the output
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // Parse either string input of file, depending on mode
    Json::Value root;
    int parseResult;
    if (this->StringInputMode) {
        parseResult = this->Internal->CanParseString(this->StringInput, root);
    } else {
        parseResult = this->Internal->CanParseFile(this->FileName, root);
    }

    if (parseResult != VTK_OK) {
        return VTK_ERROR;
    }

    // If parsed successfully into Json, then convert it into appropriate vtkPolyData
    if (root.isObject()) {
        this->Internal->ParseRoot(root, output, this->OutlinePolygons, this->SerializedPropertiesArrayName);
    }
    return VTK_OK;
}

//----------------------------------------------------------------------------
void vtkMyCityJSONReader::PrintSelf(ostream &os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << "vtkCityJSONReader" << std::endl;
    os << "Filename: " << this->FileName << std::endl;
}
