#include "vtkCityJSONReader.h"

// VTK Includes
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkCityJSONFeature.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtk_jsoncpp.h"
#include "vtksys/FStream.hxx"

// C++ includes
#include <iostream>
#include <vtkStringArray.h>
#include <iosfwd>


vtkStandardNewMacro(vtkCityJSONReader);

//----------------------------------------------------------------------------
class vtkCityJSONReader::CityJSONReaderInternal {
public:

    // Parse the Json Value corresponding to the root of the CityJSON data from the file
    static void ParseRoot(const Json::Value &root, vtkPolyData *output);

    // Verify if file exists and can be read by the parser
    // If exists, parse into Jsoncpp data structure
    static int CanParseFile(const char *filename, Json::Value &root);

};

//----------------------------------------------------------------------------
void vtkCityJSONReader::CityJSONReaderInternal::ParseRoot(const Json::Value &root, vtkPolyData *output) {
    // Initialize geometry containers
    vtkNew<vtkPoints> points;
    points->SetDataTypeToDouble();
    output->SetPoints(points);

    vtkNew<vtkCellArray> polys;
    output->SetPolys(polys);

    // Initialize object-type array used to store object type names
    vtkStringArray *objectTypeArray = vtkStringArray::New();
    objectTypeArray->SetName("object-type");
    output->GetCellData()->AddArray(objectTypeArray);
    objectTypeArray->Delete();

     // Initialize lod array used to store object type names
    vtkFloatArray *lodArray = vtkFloatArray::New();
    lodArray->SetName("lod");
    output->GetCellData()->AddArray(lodArray);
    lodArray->Delete();

    // Check type of root to ensure we're looking at a CityJSON file
    Json::Value rootType = root["type"];
    if (rootType.isNull()) {
        vtkGenericWarningMacro(<< "ParseRoot: Missing type node");
        return;
    } else if (rootType.asString() != "CityJSON") {
        vtkGenericWarningMacro(<< "ParseRoot: \"type\" is not \"CityJSON\"");
    }

    // ctkCityJSONFeature is the class that parses all the relevant data, so creating this here then calling later
    vtkNew<vtkCityJSONFeature> feature;

    // Ensure there is a 'vertices' element in this CityJSON at all, without it we can't move on
    Json::Value rootVertices = root["vertices"];
    if (!rootVertices.isArray()) {
        vtkGenericWarningMacro(<< "ParseRoot: \"vertices\" not of type \"array\"");
    } else {
        // Parse vertices
        feature->ExtractVertices(rootVertices, output);
    }

    // Ensure there is a 'cityObjects' element in this CityJSON at all, without it we can't move on
    // This element contains the definitions of objects in the CityJSON references to vertex id's
    Json::Value rootCityObjects = root["CityObjects"];
    if (rootCityObjects.isNull()) {
        vtkGenericWarningMacro(<< "ParseRoot: Missing \"CityObjects\" node");
    } else {
        // For each city Object; parse its polygons
        for (Json::Value cityObject : rootCityObjects) {
            feature->ConnectTheDots(cityObject, output);
        }
    }
}


//----------------------------------------------------------------------------
// Function that determines if the file can be parsed using a few checks
int vtkCityJSONReader::CityJSONReaderInternal::CanParseFile(const char *filename, Json::Value &root) {
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
vtkCityJSONReader::vtkCityJSONReader() {
    this->FileName = nullptr;
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
    this->Internal = new CityJSONReaderInternal;
}

//----------------------------------------------------------------------------
vtkCityJSONReader::~vtkCityJSONReader() {
    delete[] FileName;
    delete Internal;
}

//----------------------------------------------------------------------------
// "Main" function called when a file is loaded that is CityJSON
int vtkCityJSONReader::RequestData(vtkInformation * vtkNotUsed(request), vtkInformationVector ** vtkNotUsed(request), vtkInformationVector *outputVector) {
    // Get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // Get the output
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // Parse either string input or file, depending on mode
    Json::Value root;
    int parseResult;
    parseResult = this->Internal->CanParseFile(this->FileName, root);

    if (parseResult != VTK_OK) {
        return VTK_ERROR;
    }

    // If parsed successfully into Json, then convert it into appropriate vtkPolyData
    if (root.isObject()) {
        this->Internal->ParseRoot(root, output);
    }
    return VTK_OK;
}

//----------------------------------------------------------------------------
void vtkCityJSONReader::PrintSelf(ostream &os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << "vtkCityJSONReader" << std::endl;
    os << "Filename: " << this->FileName << std::endl;
}
