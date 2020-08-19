//
// Created by maarten on 11-08-20.
//

/**
 * @class   vtkCityJSONReader
 * @brief   Convert CityJSON format to vtkPolyData
 *
 * Outputs a vtkPolyData from the input
 * CityJSON Data (http://www.cityjson.org)
 */

#ifndef CITYJSONREADER_VTKCITYJSONREADER_H
#define CITYJSONREADER_VTKCITYJSONREADER_H

// VTK Includes
#include "vtkPolyDataAlgorithm.h"

class vtkPolyData;

class VTKIOCITYJSON_EXPORT vtkCityJSONReader : public vtkPolyDataAlgorithm
{
public:
    static vtkCityJSONReader* New();
    vtkTypeMacro(vtkCityJSONReader, vtkPolyDataAlgorithm);
    virtual void PrintSelf(ostream& os, vtkIndent indent) override;

    //@{
    /**
     * Accessor for name of the file that will be opened on WriteData
     */
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    //@}

    //@{
    /**
     * String used as data input (instead of file) when StringInputMode is enabled
     */
    vtkSetStringMacro(StringInput);
    vtkGetStringMacro(StringInput);
    //@}

    //@{
    /**
     * Set/get whether to use StringInput instead of reading input from file
     * The default is off
     */
    vtkSetMacro(StringInputMode, bool);
    vtkGetMacro(StringInputMode, bool);
    vtkBooleanMacro(StringInputMode, bool);
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
    vtkCityJSONReader();
    ~vtkCityJSONReader() override;

    //@{
    /**
     * Core implementation of the
     */
    int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;
    char* FileName;
    char* StringInput;
    bool StringInputMode;
    bool TriangulatePolygons;
    bool OutlinePolygons;
    char* SerializedPropertiesArrayName;
    //@}

private:
    class CityJSONReaderInternal;
    CityJSONReaderInternal* Internal;

    vtkCityJSONReader(const vtkCityJSONReader&) = delete;
    void operator=(const vtkCityJSONReader&) = delete;
};

#endif //CITYJSONREADER_VTKCITYJSONREADER_H

