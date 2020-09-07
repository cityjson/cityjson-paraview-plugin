paraview_add_plugin(CityJSONReader
        VERSION "1.0"
        MODULES VTK::IOCityJSON
        MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CityJSONReader/vtk.module"
        )

option(BUILD_TESTING "Build Plugin Testing" OFF)
if (BUILD_TESTING)
    add_subdirectory(Testing)
endif()
