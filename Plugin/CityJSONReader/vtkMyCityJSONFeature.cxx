#include "vtkMyCityJSONFeature.h"

// VTK Includes
#include "vtkAbstractArray.h"
#include "vtkBitArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkLine.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkPolygon.h"
#include "vtkStringArray.h"

#include <sstream>
#include <string>

vtkStandardNewMacro(vtkMyCityJSONFeature);

namespace {
    vtkOStreamWrapper &operator<<(vtkOStreamWrapper &os, const Json::Value &root) {
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "All";
        builder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

        writer->write(root, &os.GetOStream());
        return os;
    }
}

//----------------------------------------------------------------------------
vtkMyCityJSONFeature::vtkMyCityJSONFeature() {
    this->FeatureId = nullptr;
    this->OutlinePolygons = false;
}

//----------------------------------------------------------------------------
vtkMyCityJSONFeature::~vtkMyCityJSONFeature() {
    free(this->FeatureId);
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::CreatePoint(const Json::Value &coordinates, double point[3]) {
    // Check if Coordinates corresponds to Point
    if (!IsPoint(coordinates)) {
        vtkErrorMacro(<< "Wrong data format for a point!");
        return false;
    }

    // Do isDouble before asDouble to prevent inconsistency
    // Probably check for float/int too

    if (coordinates.size() == 1) {
        // Update the 3D Coordinates using the 1 Value in the array and rest of the 2 as 0
        Json::Value x = coordinates[0];
        point[0] = x.asDouble();
        point[1] = 0;
        point[2] = 0;
    } else if (coordinates.size() == 2) {
        // Update the 3D Coordinates using the 2 Values in the array and 3rd as 0
        Json::Value x = coordinates[0];
        Json::Value y = coordinates[1];
        point[0] = x.asDouble();
        point[1] = y.asDouble();
        point[2] = 0;
    } else if (coordinates.size() == 3) {
        // Update the 3D Coordinates using the 3 Values in the array
        Json::Value x = coordinates[0];
        Json::Value y = coordinates[1];
        Json::Value z = coordinates[2];
        point[0] = x.asDouble();
        point[1] = y.asDouble();
        point[2] = z.asDouble();
    }

    // Return that we properly created the point
    return true;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractPoint(const Json::Value &coordinates, vtkPolyData *outputData) {
    // Check if Coordinates corresponds to Single Point
    if (!IsPoint(coordinates)) {
        vtkErrorMacro(<< "Wrong data format for a point!");
        return nullptr;
    }

    // Obtain point data from Json structure and add to outputData
    double point[3];
    CreatePoint(coordinates, point);

    const int PID_SIZE = 1;
    vtkIdType pid;

    vtkPoints *points = outputData->GetPoints();
    pid = points->InsertNextPoint(point);

    vtkCellArray *verts = outputData->GetVerts();
    verts->InsertNextCell(PID_SIZE, &pid);

    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("feature-id");
    auto *ids = vtkArrayDownCast<vtkStringArray>(array);
    ids->InsertNextValue(this->FeatureId);

    return outputData;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractMultiPoint(const Json::Value &coordinates, vtkPolyData *outputData) {
    // Check if Coordinates corresponds to Multi Points
    if (!IsMultiPoint(coordinates)) {
        vtkErrorMacro(<< "Wrong data format for a Multi Point!");
        return nullptr;
    }

    if (coordinates.isArray()) {
        vtkPoints *points = outputData->GetPoints(); // Contain the locations of the points
        vtkCellArray *verts =
                outputData->GetVerts(); // Contain the indices corresponding to the position of the vertices

        vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("feature-id");
        vtkStringArray *ids = vtkArrayDownCast<vtkStringArray>(array);

        const int PID_SIZE = coordinates.size();
        vtkIdType *pids = new vtkIdType[PID_SIZE];
        double point[3];
        for (int i = 0; i < PID_SIZE; i++) {
            // Parse point from Json object to double array and add it to the points array
            CreatePoint(coordinates, point);
            pids[i] = points->InsertNextPoint(point);
        }

        // Update polyData vertices to store multiple points
        verts->InsertNextCell(PID_SIZE, pids);
        ids->InsertNextValue(this->FeatureId);

        delete[] pids;
    }

    return outputData;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractLineString(
        const Json::Value &coordinates, vtkPolyData *outputData) {
    // Check if Coordinates corresponds to Line String
    if (!IsLineString(coordinates)) {
        vtkErrorMacro(<< "Wrong data format for a Line String!");
        return nullptr;
    }

    vtkPoints *points = outputData->GetPoints();
    vtkNew<vtkPolyLine> polyLine;
    double xyz[3];
    vtkIdList *pointIdList = polyLine->GetPointIds();
    vtkIdType pointId;
    for (Json::Value::ArrayIndex i = 0; i < coordinates.size(); i++) {
        this->CreatePoint(coordinates[i], xyz);
        pointId = points->InsertNextPoint(xyz);
        pointIdList->InsertNextId(pointId);
    }
    outputData->GetLines()->InsertNextCell(polyLine);
    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("feature-id");
    vtkStringArray *ids = vtkArrayDownCast<vtkStringArray>(array);
    ids->InsertNextValue(this->FeatureId);

    return outputData;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractMultiLineString(const Json::Value &coordinateArray, vtkPolyData *outputData) {
    // Check if Coordinate Array corresponds to Multi Line String
    if (!IsMultiLineString(coordinateArray)) {
        vtkErrorMacro(<< "Wrong data format for a Multi Line String!");
        return nullptr;
    }

    for (Json::Value::ArrayIndex i = 0; i < coordinateArray.size(); i++) {
        this->ExtractLineString(coordinateArray[i], outputData);
    }

    return outputData;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractPolygon(const Json::Value &coordinate, vtkPolyData *outputData) {
    // Check if Coordinate Array corresponds to Polygon
    if (!IsPolygon(coordinate)) {
        vtkErrorMacro(<< "Wrong data format for a Polygon!");
        return nullptr;
    }

    bool POLYGON_WITH_HOLES = coordinate.size() > 1 ? true : false;

    vtkPoints *points = outputData->GetPoints();
    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("feature-id");
    vtkStringArray *ids = vtkArrayDownCast<vtkStringArray>(array);

    // Output is either vtkPolygon or vtkPolyLine,
    // depending on OutputPolygons option.
    vtkCellArray *polys = nullptr;
    vtkCell *exteriorPoly = nullptr;
    if (this->OutlinePolygons) {
        polys = outputData->GetLines();
        exteriorPoly = vtkPolyLine::New();
    } else {
        polys = outputData->GetPolys();
        exteriorPoly = vtkPolygon::New();
    }

    // For exterior Polygon
    Json::Value exteriorPolygon = coordinate[0];

    int EXTERIOR_POLYGON_VERTEX_COUNT = exteriorPolygon.size() - 1;
    exteriorPoly->GetPointIds()->SetNumberOfIds(EXTERIOR_POLYGON_VERTEX_COUNT);

    double point[3];
    // Remember first point, if needed for polyline
    this->CreatePoint(exteriorPolygon[0], point);
    vtkIdType idPoint0 = points->InsertNextPoint(point);
    exteriorPoly->GetPointIds()->SetId(0, idPoint0);

    // Add points for rest of polygon
    for (int i = 1; i < EXTERIOR_POLYGON_VERTEX_COUNT; i++) {
        this->CreatePoint(exteriorPolygon[i], point);
        vtkIdType id = points->InsertNextPoint(point);
        exteriorPoly->GetPointIds()->SetId(i, id);
    }

    // For outline mode, add first point to the end
    if (this->OutlinePolygons) {
        exteriorPoly->GetPointIds()->InsertNextId(idPoint0);
    }

    polys->InsertNextCell(exteriorPoly);
    ids->InsertNextValue(this->FeatureId);
    exteriorPoly->Delete();

    if (!POLYGON_WITH_HOLES)
        return outputData;

    // Todo Modify polydata to support polygon with holes

    return outputData;
}

//----------------------------------------------------------------------------
vtkPolyData *vtkMyCityJSONFeature::ExtractMultiPolygon(const Json::Value &coordinateArray, vtkPolyData *outputData) {
    // Check if Coordinate Array corresponds to Multi Polygon
    if (!IsMultiPolygon(coordinateArray)) {
        vtkErrorMacro(<< "Wrong data format for a Multi Polygon!");
        return nullptr;
    }

    for (const auto &i : coordinateArray) {
        // Extract polygon into different polyData and append into a common polyData using the
        // appendPolyData Filter
        this->ExtractPolygon(i, outputData);
    }

    return outputData;
}

void vtkMyCityJSONFeature::ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData) {
    this->verticesRoot = vertices;

    for (Json::Value vertex : vertices){
        this->ExtractPoint(vertex, outputData);
    }
}

//----------------------------------------------------------------------------
void vtkMyCityJSONFeature::ExtractCityJSONFeature(const Json::Value &root, vtkPolyData *outputData) {
    this->featureRoot = root;

    // Check that type is Feature
    Json::Value typeNode = root["type"];
    if (typeNode.isNull() || "Feature" != typeNode.asString()) {
        vtkErrorMacro(<< "Unknown type. \"Feature\" expected");
        return;
    }

    // Check for geometry node
    Json::Value geometryNode = root["geometry"];
    if (geometryNode.isNull()) {
        vtkErrorMacro(<< "Missing geometry node");
        return;
    }

    // Check for properties node
    Json::Value propertiesNode = root["properties"];
    if (propertiesNode.isNull()) {
        vtkErrorMacro(<< "Missing properties node");
        return;
    }

    // Check for feature id
    std::string featureString;
    Json::Value idNode = root["id"];
    // No Json::Value::toString() method, so homebrew one here
    std::stringstream oss;
    switch (idNode.type()) {
        case Json::nullValue:
            break;

        case Json::intValue:
        case Json::uintValue:
            oss << idNode.asInt();
            featureString = oss.str();
            break;

        case Json::realValue:
            oss << idNode.asDouble();
            featureString = oss.str();
            break;

        case Json::stringValue:
            featureString = idNode.asString();
            break;

        default:
            vtkWarningMacro(<< "Unsupported \"id\" type: " << idNode.type());
            break;
    }

    this->FeatureId = strdup(featureString.c_str());
    this->ExtractCityJSONFeatureGeometry(geometryNode, outputData);
}

//----------------------------------------------------------------------------
void vtkMyCityJSONFeature::ExtractCityJSONFeatureGeometry(const Json::Value &geometryRoot, vtkPolyData *outputData) {
    // Check for geometry-type node
    Json::Value geometryTypeNode = geometryRoot["type"];
    if (geometryTypeNode.isNull()) {
        vtkErrorMacro(<< "Missing geometry-type node");
        return;
    }
    if (!geometryTypeNode.isString()) {
        vtkErrorMacro(<< "Invalid geometry-type node");
        return;
    }

    std::string typeString = geometryTypeNode.asString();
    if (typeString == CityJSON_GEOMETRY_COLLECTION) {
        // For GeometryCollection
        Json::Value geometries = geometryRoot["geometries"];
        for (Json::Value::ArrayIndex i = 0; i < geometries.size(); i++) {
            Json::Value child = geometries[i];
            this->ExtractCityJSONFeatureGeometry(child, outputData);
        }
        return;
    }

    // (else)
    Json::Value coordinates = geometryRoot["coordinates"];
    if (typeString == CityJSON_POINT) {
        this->ExtractPoint(coordinates, outputData);
    } else if (typeString == CityJSON_MULTI_POINT) {
        this->ExtractMultiPoint(coordinates, outputData);
    } else if (typeString == CityJSON_LINE_STRING) {
        this->ExtractLineString(coordinates, outputData);
    } else if (typeString == CityJSON_MULTI_LINE_STRING) {
        this->ExtractMultiLineString(coordinates, outputData);
    } else if (typeString == CityJSON_POLYGON) {
        this->ExtractPolygon(coordinates, outputData);
    } else if (typeString == CityJSON_MULTI_POLYGON) {
        this->ExtractMultiPolygon(coordinates, outputData);
    } else {
        vtkErrorMacro(<< "Unknown or unsupported geometry type " << geometryTypeNode);
    }
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsLineString(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro(<< "Expected Arrays as input for point at " << root);
        return false;
    }

    if (root.size() < 1) {
        vtkErrorMacro(<< "Expected at least 1 value at " << root);
        return false;
    }

    for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
        Json::Value child = root[i];
        if (!IsPoint(child)) {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsMultiLineString(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro(<< "Expected Array as input for point at " << root);
        return false;
    }

    if (root.size() < 1) {
        vtkErrorMacro(<< "Expected at least 1 value at " << root);
        return false;
    }

    for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
        Json::Value child = root[i];
        if (!IsLineString(child)) {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsPoint(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro("Expected Array as input for point at " << root);
        return false;
    }

    if (!(!root.empty() && root.size() < 4)) {
        vtkErrorMacro(<< "Expected 3 or less dimension values at " << root << " for point");
        return false;
    }

    for (const auto &child : root) {
        if (!child.isNumeric()) {
            vtkErrorMacro(<< "Value not Numeric as expected at " << child);
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsMultiPoint(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro(<< "Expected Array as input for multi point at " << root);
        return false;
    }

    if (root.size() < 1) {
        vtkErrorMacro(<< "Expected at least 1 value at " << root << " for multipoint");
        return false;
    }

    for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
        Json::Value child = root[i];
        if (!IsPoint(child)) {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsPolygon(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro(<< "Expected Array as input for polygon at " << root);
        return false;
    }

    if (root.size() < 1) {
        vtkErrorMacro(<< "Expected at least 1 value at " << root << "for polygon");
        return false;
    }

    for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
        Json::Value child = root[i];
        if (!IsLineString(child)) {
            return false;
        }
        // First and last element of child must be same to complete the polygon loop.
        // Check not added.
    }

    return true;
}

//----------------------------------------------------------------------------
bool vtkMyCityJSONFeature::IsMultiPolygon(const Json::Value &root) {
    if (!root.isArray()) {
        vtkErrorMacro(<< "Expected Array as input for multi polygon at " << root);
        return false;
    }

    if (root.size() < 1) {
        vtkErrorMacro(<< "Expected at least 1 value at " << root << " for multi polygon");
        return false;
    }

    for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
        Json::Value child = root[i];
        if (!IsPolygon(child)) {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
void vtkMyCityJSONFeature::PrintSelf(ostream &os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << indent << "vtkCityJSONFeature" << std::endl;
    os << indent << "Root: ";

    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "All";
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    writer->write(this->featureRoot, &os);
}
