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
     * Specify file name of the CityJSON data file to read.
     */
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    //@}

protected:
    vtkMyCityJSONReader();
    ~vtkMyCityJSONReader() override;

    int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override;

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
