set(classes vtkMyCityJSONReader vtkMyCityJSONFeature)

vtk_module_add_module(VTK::IOCityJSON CLASSES ${classes})

paraview_add_server_manager_xmls(XMLS MyCityJSONReader.xml)
