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

vtkMyCityJSONFeature::vtkMyCityJSONFeature() = default;

//----------------------------------------------------------------------------
vtkMyCityJSONFeature::~vtkMyCityJSONFeature() = default;

// NEW ----
vtkPolyData *vtkMyCityJSONFeature::ConnectTheDots(const Json::Value &cityObject, vtkPolyData *outputData) {

    if (cityObject.isNull()){
        vtkErrorMacro(<< "Geometry node is missing!");
        return nullptr;
    }

    for (Json::Value boundary : cityObject["geometry"][0]["boundaries"][0]) {
        for (Json::Value vertices : boundary) {

            vtkCellArray *polys = outputData->GetPolys();
            vtkNew<vtkPolygon> poly;
            vtkIdList* polyPointIds = poly->GetPointIds();

            if (vertices.empty()) {
                continue;
            }

            // For each vertex id in the boundary list, insert and add to poly
            for (Json::Value boundaryVertexId : vertices){
                polyPointIds->InsertNextId(boundaryVertexId.asInt());
            }

            polyPointIds->SetNumberOfIds(vertices.size());
            polys->InsertNextCell(poly);

        }
    }

    return outputData;
}


// NEW ----
void vtkMyCityJSONFeature::ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData) {

    vtkPoints *points = outputData->GetPoints();
    vtkCellArray* verts = outputData->GetVerts();

    vtkAbstractArray *array = outputData->GetCellData()->GetAbstractArray("vertex-id");
    auto *ids = vtkArrayDownCast<vtkIntArray>(array);

    for (Json::Value vertex : vertices){
        this->allVertices.push_back(std::vector<double>{vertex[0].asDouble(), vertex[1].asDouble(), vertex[2].asDouble()});
    }

    points->SetNumberOfPoints(allVertices.size());

    int currentVertexId = 0;
    for (auto vertex : allVertices) {
        points->InsertPoint(currentVertexId, vertex[0], vertex[1], vertex[2]);

        // Uncomment to show vertices
//        vtkIdType id = currentVertexId;
//        verts->InsertNextCell(1, &id);

        ids->InsertNextValue(currentVertexId);
        currentVertexId++;
    }

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
