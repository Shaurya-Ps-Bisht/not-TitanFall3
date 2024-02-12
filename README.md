# not-TitanFall3

Just like the title suggests, this is not TitanFall 3, but an OpenGL renderer in C++. 

[Demo Video](https://drive.google.com/drive/folders/1S937oZShlE9I39EH9XH1TvmUX2pzqE3o)
 
![Alt text](res/Screenshots/1.png)

## Features

* Physically based shading
* Terrain Generation using tesselation shaders and heightmaps
* Directional and point lights
* Shadows
* Cascaded shadow mapping
* Fogs/Clouds rendering using Ray Marching
* Model Loading (gltf, obj) with animations
* Instanced rendering

## How to run

* Not using make or any alternative for builds at the moment.
* Should work on Windows 10, both release and debug mode in Visual Studio.

## To Do

* resolve model loading missing weights and size difference (animated vs still)
* Material Class
* MetallicRoughness combined in gltf improvement
* Depth pass for instanced objects, animations and terrain
* Point light shadows performance(Bounding Volume and non-moving objects)
* Spotlights
* Reflection Probes
* OIT
* Proper material/texture access in model class
* learn Direct State Access

# God bless RenderDOc
