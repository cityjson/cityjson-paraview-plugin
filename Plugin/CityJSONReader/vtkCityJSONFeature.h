#ifndef vtkCityJSONFeature_h
#define vtkCityJSONFeature_h

// VTK Includes
#include "vtkDataObject.h"
#include "vtkIOCityJSONModule.h" // For export macro
#include "vtk_jsoncpp.h"        // For json parser

class vtkPolyData;

class VTKIOCITYJSON_EXPORT vtkCityJSONFeature : public vtkDataObject {
public:
    static vtkCityJSONFeature *New();

    void PrintSelf(ostream &os, vtkIndent indent) override;

vtkTypeMacro(vtkCityJSONFeature, vtkDataObject);

    static void ExtractVertices(const Json::Value &vertices, vtkPolyData *outputData);

    vtkPolyData *ConnectTheDots(const Json::Value &cityObject, vtkPolyData *outputData);

protected:
    vtkCityJSONFeature();

    ~vtkCityJSONFeature() override;

/**
 * Json::Value featureRoot corresponds to the root of the CityJSON feature
 * from which the geometry and properties are to be extracted
 */
    Json::Value featureRoot;

private:
    vtkCityJSONFeature(const vtkCityJSONFeature &) = delete;

    void operator=(const vtkCityJSONFeature &) = delete;
};

#endif // vtkCityJSONFeature_h
