cmake_minimum_required(VERSION 3.8)
project(CityJSONReader)

find_package(ParaView REQUIRED)

set("_paraview_build_plugin" CityJSONReader)

paraview_add_plugin(CityJSONReader
        REQUIRED_ON_SERVER
        VERSION "1.0"
        MODULES CityJSONReader::vtkCityJSONReader
        MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Reader/vtk.module")

target_link_libraries(CityJSONReader
        PRIVATE
        ParaView::RemotingServerManager)

if (BUILD_TESTING)
    add_subdirectory(Testing)
endif ()
