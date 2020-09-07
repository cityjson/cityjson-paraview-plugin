#ifndef vtkMyCityJSONReader_h
#define vtkMyCityJSONReader_h

#include "vtkIOCityJSONModule.h" // for export macro
#include "vtkPolyDataAlgorithm.h"

class vtkPolyData;

class VTKIOCITYJSON_EXPORT vtkMyCityJSONReader : public vtkPolyDataAlgorithm
{
public:
    static vtkMyCityJSONReader* New();
    vtkTypeMacro(vtkMyCityJSONReader, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    //@{
    /**
     * Specify file name of the CityGML data file to read.
     */
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    //@}

    //@{
    /**
     * Set/get whether to convert all output polygons to triangles.
     * Note that if OutinePolygons mode is on, then no output polygons
     * are generated, and in that case, this option is not relevant.
     * The default is off.
     */
    vtkSetMacro(TriangulatePolygons, bool);
    vtkGetMacro(TriangulatePolygons, bool);
    vtkBooleanMacro(TriangulatePolygons, bool);
    //@}

    //@{
    /**
     * Set/get option to generate the border outlining each polygon,
     * so that the output cells for polygons are vtkPolyLine instances.
     * The default is off.
     */
    vtkSetMacro(OutlinePolygons, bool);
    vtkGetMacro(OutlinePolygons, bool);
    vtkBooleanMacro(OutlinePolygons, bool);
    //@}

    //@{
    /**
     * Set/get name of data array for serialized GeoJSON "properties" node.
     * If specified, data will be stored as vtkCellData/vtkStringArray.
     */
    vtkSetStringMacro(SerializedPropertiesArrayName);
    vtkGetStringMacro(SerializedPropertiesArrayName);
    //@}

    /**
   * Specify feature property to read in with geometry objects
   * Note that defaultValue specifies both type & value
   */
    void AddFeatureProperty(const char* name, vtkVariant& typeAndDefaultValue);

protected:
    vtkMyCityJSONReader();
    ~vtkMyCityJSONReader() override;

    int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;

    char* FileName;
    char* StringInput;
    bool StringInputMode;
    bool TriangulatePolygons;
    bool OutlinePolygons;
    char* SerializedPropertiesArrayName;

private:
    class CityJSONReaderInternal;
    CityJSONReaderInternal* Internal;

    vtkMyCityJSONReader(const vtkMyCityJSONReader&) = delete;
    void operator=(const vtkMyCityJSONReader&) = delete;
};

#endif
