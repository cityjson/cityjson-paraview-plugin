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

//@{
/**
 * Set/get option to generate the border outlining each polygon,
 * so that resulting cells are vtkPolyLine instead of vtkPolygon.
 * The default is off
 */
    vtkSetMacro(OutlinePolygons, bool);

    vtkGetMacro(OutlinePolygons, bool);

    vtkBooleanMacro(OutlinePolygons, bool);
//@}


    //CityJSON
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

/**
 * Id of current CityJSON feature being parsed
 */
    char *FeatureId;

    int vertexId = 0;

    std::vector<std::vector<double>> allVertices;

/**
 * Set/get option to generate the border outlining each polygon,
 * so that the output cells are polyine data.
 */
    bool OutlinePolygons;

/**
 * Check if the root contains corresponding appropriate geometry in the
 * Jsoncpp root
 */
    bool IsPoint(const Json::Value &root);

//@}

/**
 * Point[] from its JSON equivalent
 */
    bool CreatePoint(const Json::Value &coordinates, double point[3]);

private:
    vtkMyCityJSONFeature(const vtkMyCityJSONFeature &) = delete;

    void operator=(const vtkMyCityJSONFeature &) = delete;
};

#endif // vtkCityJSONFeature_h
