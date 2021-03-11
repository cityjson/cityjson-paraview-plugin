#ifndef vtkCityJSONReader_h
#define vtkCityJSONReader_h

#include "vtkIOCityJSONModule.h" // for export macro
#include "vtkPolyDataAlgorithm.h"

class vtkPolyData;

class VTKIOCITYJSON_EXPORT vtkCityJSONReader : public vtkPolyDataAlgorithm
{
public:
    static vtkCityJSONReader* New();
    vtkTypeMacro(vtkCityJSONReader, vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    //@{
    /**
     * Specify file name of the CityJSON data file to read.
     */
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    //@}

protected:
    vtkCityJSONReader();
    ~vtkCityJSONReader() override;

    int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override;

    char* FileName;

private:
    class CityJSONReaderInternal;
    CityJSONReaderInternal* Internal;

    vtkCityJSONReader(const vtkCityJSONReader&) = delete;
    void operator=(const vtkCityJSONReader&) = delete;
};

#endif
