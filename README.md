[![GitHub license](https://img.shields.io/github/license/mdjong1/CityJSON-Reader-ParaView)](https://github.com/mdjong1/CityJSON-Reader-ParaView/blob/master/LICENSE)

# CityJSON Reader Plugin for ParaView
A plugin that allows you to read CityJSON files directly into ParaView for modeling.

![image](https://user-images.githubusercontent.com/4410453/110961288-e904e800-834f-11eb-8e09-e7fb65857524.png)

https://github.com/cityjson  
https://www.cityjson.org/  
https://3d.bk.tudelft.nl/


# How to use

1. Download the latest binary [here](https://github.com/mdjong1/CityJSON-Reader-ParaView/releases) (Linux only at the moment)
2. Open up ParaView
3. Go to `Tools->Manage Plugins...->Load New ...`
4. Choose the downloaded CityJSONReader plugin
5. Load whatever CityJSON file you want

# How to compile

1. Get a copy of [ParaView](https://gitlab.kitware.com/paraview/paraview)
2. Build ParaView (see their [instructions](https://www.paraview.org/Wiki/ParaView:Build_And_Install) if necessary)
3. Get a copy of [this repo](https://github.com/mdjong1/CityJSON-Reader-ParaView)
4. Build this:
    1. Go to the folder you have the source to this plugin
    2. `mkdir build`
    3. `cmake .. -DParaView_DIR=<PATH TO BUILT PARAVIEW>`
    4. `cmake --build ..`
    5. `make`
5. You can find your built plugin in `lib\plugins\CityJSONReader` as .dll for Windows .so for Mac/Linux

