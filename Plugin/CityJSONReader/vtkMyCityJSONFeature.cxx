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

// Get boundaries for each object and insert polygons that reference each inserted point
vtkPolyData *vtkMyCityJSONFeature::ConnectTheDots(const Json::Value &cityObject, vtkPolyData *outputData) {

    if (cityObject.isNull()){
        vtkErrorMacro(<< "Geometry node is missing!");
        return nullptr;
    }

    for (Json::Value geometry : cityObject["geometry"]) {
        for (Json::Value boundary : geometry["boundaries"]) {
            for (Json::Value element : boundary) {
                for (Json::Value vertices : element) {

                    vtkCellArray *polys = outputData->GetPolys();
                    vtkNew<vtkPolygon> poly;
                    vtkIdList *polyPointIds = poly->GetPointIds();

                    if (vertices.empty()) {
                        continue;
                    }

                    // For each vertex id in the boundary list, insert and add to poly
                    for (Json::Value boundaryVertexId : vertices) {
                        polyPointIds->InsertNextId(boundaryVertexId.asInt());
                    }

                    polyPointIds->SetNumberOfIds(vertices.size());
                    polys->InsertNextCell(poly);

                }
            }
        }
    }

    return outputData;
}


// Extract all vertices and insert them into points. Optionally also as (visible) vertices
void vtkMyCityJSONFeature::ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData) {

    vtkPoints *points = outputData->GetPoints();
    vtkCellArray* verts = outputData->GetVerts();

    for (Json::Value vertex : vertices) {
        vtkIdType vertexId = points->InsertNextPoint(vertex[0].asDouble(), vertex[1].asDouble(), vertex[2].asDouble());

        // Comment to hide vertices
        verts->InsertNextCell(1, &vertexId);
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
