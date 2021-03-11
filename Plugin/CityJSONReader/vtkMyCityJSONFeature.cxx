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

// NEW ----
vtkPolyData *vtkMyCityJSONFeature::ConnectTheDots(const Json::Value &cityObject, vtkPolyData *outputData) {

    if (cityObject.isNull()){
        vtkErrorMacro(<< "Geometry node is missing!");
        return nullptr;
    }

//    const int PID_SIZE = 1;

//    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("vertex-id");
//    auto *ids = vtkArrayDownCast<vtkIntArray>(array);

    for (Json::Value boundary : cityObject["geometry"][0]["boundaries"][0]) {
        for (Json::Value vertices : boundary) {

//            vtkPoints *points = outputData->GetPoints();

            vtkCellArray *polys = outputData->GetPolys();
            vtkCell *exteriorPoly = vtkPolygon::New();

            Json::Value vertexIds = vertices;

            // Define how many points this poly has
            exteriorPoly->GetPointIds()->SetNumberOfIds(vertexIds.size());

            // For each vertex id in the boundary list, insert and add to poly
            for (Json::Value boundaryVertexId : vertexIds){

                int vId = boundaryVertexId.asInt();

//                double point[3] = {this->allVertices[vId][0], this->allVertices[vId][1], this->allVertices[vId][2]};
//
//                std::cout << boundaryVertexId << ": " << point[0] << " " << point[1] << " " << point[2] << std::endl;
//
//                vtkIdType id = points->InsertNextPoint(point);

                exteriorPoly->GetPointIds()->InsertNextId(vId);

//                vtkCellArray* verts = outputData->GetVerts();
//                verts->InsertNextCell(PID_SIZE, &id);

//                ids->InsertNextValue(this->vertexId);
//                this->vertexId++;

//                exteriorPoly->GetPointIds()->SetId(i, vId);
            }

            polys->InsertNextCell(exteriorPoly);
            exteriorPoly->Delete();
        }
    }

    std::cout << "Finished connecting dots" << std::endl;

    return outputData;
}


// NEW ----
void vtkMyCityJSONFeature::ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData) {

    vtkPoints *points = outputData->GetPoints();

    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("vertex-id");
    auto *ids = vtkArrayDownCast<vtkIntArray>(array);

    for (Json::Value vertex : vertices){
        this->allVertices.push_back(std::vector<double>{vertex[0].asDouble(), vertex[1].asDouble(), vertex[2].asDouble()});
    }

    points->SetNumberOfPoints(allVertices.size());

    int currentVertexId = 0;
    for (auto vertex : allVertices) {
        points->InsertPoint(currentVertexId, vertex[0], vertex[1], vertex[2]);
        ids->InsertNextValue(currentVertexId);
        currentVertexId++;
    }

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
