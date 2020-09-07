#ifndef vtkMyCityJSONFeature_h
#define vtkMyCityJSONFeature_h

// VTK Includes
#include "vtkDataObject.h"
#include "vtkIOCityJSONModule.h" // For export macro
#include "vtk_jsoncpp.h"        // For json parser

class vtkPolyData;

// Currently implemented CityJSON compatible Geometries
#define CityJSON_POINT "Point"
#define CityJSON_MULTI_POINT "MultiPoint"
#define CityJSON_LINE_STRING "LineString"
#define CityJSON_MULTI_LINE_STRING "MultiLineString"
#define CityJSON_POLYGON "Polygon"
#define CityJSON_MULTI_POLYGON "MultiPolygon"
#define CityJSON_GEOMETRY_COLLECTION "GeometryCollection"

class VTKIOCITYJSON_EXPORT vtkMyCityJSONFeature : public vtkDataObject {
public:
    static vtkMyCityJSONFeature *New();

    virtual void PrintSelf(ostream &os, vtkIndent indent) override;

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

/**
 * Extract the geometry corresponding to the CityJSON feature stored at root
 * Assign any feature properties passed as cell data
 */
    void ExtractCityJSONFeature(const Json::Value &root, vtkPolyData *outputData);

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

/**
 * Set/get option to generate the border outlining each polygon,
 * so that the output cells are polyine data.
 */
    bool OutlinePolygons;

/**
 * Extract CityJSON geometry into vtkPolyData *
 */
    void ExtractCityJSONFeatureGeometry(const Json::Value &root, vtkPolyData *outputData);

//@{
/**
 * In extractXXXX() Extract CityJSON geometries XXXX into outputData
 */
    vtkPolyData *ExtractPoint(const Json::Value &coordinates, vtkPolyData *outputData);

    vtkPolyData *ExtractLineString(const Json::Value &coordinates, vtkPolyData *outputData);

    vtkPolyData *ExtractPolygon(const Json::Value &coordinates, vtkPolyData *outputData);
//@}

//@{
/**
 * extractMultiXXXX extracts an array of geometries XXXX into the outputData
 */
    vtkPolyData *ExtractMultiPoint(const Json::Value &coordinates, vtkPolyData *outputData);

    vtkPolyData *ExtractMultiLineString(const Json::Value &coordinates, vtkPolyData *outputData);

    vtkPolyData *ExtractMultiPolygon(const Json::Value &coordinates, vtkPolyData *outputData);
//@}

//@{
/**
 * Check if the root contains corresponding appropriate geometry in the
 * Jsoncpp root
 */
    bool IsPoint(const Json::Value &root);

    bool IsMultiPoint(const Json::Value &root);

    bool IsLineString(const Json::Value &root);      // To Do.
    bool IsMultiLineString(const Json::Value &root); // To Do.
    bool IsPolygon(const Json::Value &root);         // To Do.
    bool IsMultiPolygon(const Json::Value &root);    // To Do.
//@}

/**
 * Point[] from its JSON equivalent
 */
    bool CreatePoint(const Json::Value &coordinates, double point[3]);

    void InsertFeatureProperties(vtkPolyData *outputData);

private:
    vtkMyCityJSONFeature(const vtkMyCityJSONFeature &) = delete;

    void operator=(const vtkMyCityJSONFeature &) = delete;
};

#endif // vtkCityJSONFeature_h
