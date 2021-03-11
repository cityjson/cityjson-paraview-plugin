#ifndef vtkMyCityJSONFeature_h
#define vtkMyCityJSONFeature_h

// VTK Includes
#include "vtkDataObject.h"
#include "vtkIOCityJSONModule.h" // For export macro
#include "vtk_jsoncpp.h"        // For json parser

class vtkPolyData;

class VTKIOCITYJSON_EXPORT vtkMyCityJSONFeature : public vtkDataObject {
public:
    static vtkMyCityJSONFeature *New();

    void PrintSelf(ostream &os, vtkIndent indent) override;

vtkTypeMacro(vtkMyCityJSONFeature, vtkDataObject);

    void ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData);

    vtkPolyData *ConnectTheDots(const Json::Value &cityObject, vtkPolyData *outputData);

protected:
    vtkMyCityJSONFeature();

    ~vtkMyCityJSONFeature() override;

/**
 * Json::Value featureRoot corresponds to the root of the CityJSON feature
 * from which the geometry and properties are to be extracted
 */
    Json::Value featureRoot;

    std::vector<std::vector<double>> allVertices;

private:
    vtkMyCityJSONFeature(const vtkMyCityJSONFeature &) = delete;

    void operator=(const vtkMyCityJSONFeature &) = delete;
};

#endif // vtkCityJSONFeature_h
