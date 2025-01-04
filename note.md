# OpenGL Project keybinds

 - 'ZQSD' for movement (azerty keyboard)
 - 'Y' for wireframe toggle
 - 'T' for chair animation toggle
 - 'R' for light 1 toggle

# 10-Nov-2024
currently trying to separate the main file into several more digestable parts, for ease of maintenance and readability.
Otherwise, the direcitonal light + shadows work, and the point light + shadows work.
The textures and normal maps work, and there are python converters for 8bit and normal map PNGs.
# 11-Nov-2024
The separation of the main file into several parts is done, and the code is now more readable.
The main.cpp file is still very large, but still better than before. Did the 2 rooms in minecraft 64x64 texture blocks, subject to change for more textures, like a minecraft house.

next steps will include :

 - several point lights and shadows for each.
 - roughness maps
 - import obj models
 - do the 2 rooms in minecraft 64x64 texture blocks
 - figure out the different vibes in each room (different shaders ?)

# 30-Dec-2024
I have been working on the project, but not updating the notes.
In the meantime, I did the dual shader pipeline, the rrom2 shader is just a color inversion of the room1 shader, as proof of concept, I'll look into a dither effect or a toon shader.
Also did the multiple point lights, they are also dynamic, can change color and position during the while loop. I did not implement shadows for these extra point lights, for both performance and complexity reasons, as the shadows are already a bonus feature.
Wireframe toggle has been added.

# 2-Jan-2025
Material struct, more adapted than the given one in Geometry.hpp.
Added Specular Maps, gives good enough results, thanks to a mix of diffuse, normal and specular maps.
Room 2 has transparency, and a dither effect, hard coded monochrome dither on a Bayer matrix, the color and size, and mono boolean can be changed in the shader code.
Added a skybox, more precisely a spheremap, and runnign the app randomly chooses one of 4 space spheremaps.
For randomization, I use a time seed, otherwise the same spheremap is chosen every time the app is run.
Added small spheres to each light source, to see their position, and color. They are not affected by the room shaders, as it beats the purpose of the different lights in each room, granted it can look weird in room 2 with the dither effect. 

# 4-Jan-2025
Better omnidirectional transmisison for transparent objects, and the transparency is more realistic, with weighted normales and inverse normals, to mimic direct refraction.
new vertex shader on room 2 for gravity pull.warp effect on the triangles, distance based on vertex position and all light positions, as if mini suns with great mass. going hand in hand with the space spheremaps, its now a space themed room, in minecraft blocks, but with better visuals.

## note

If i implemente a big thing, i can try to test it in APP2, to keep APP1 working on previous version. (now on APP3_executable)

# How to run the thing

```bash
mkdir bin; cd bin; cmake ..; make -j4; ./APP3_executable
```

assignment : 

---

# Features to Implement

 - C++ application with OpenGL 3+ (shaders, no fixed pipeline) ✅
 - Windowed app with event loop ✅
 - Must compile on Linux (no memory leaks, documented, uses Git). ❌ (MacOS, no Valgrind)
 - Loading/importing of geometry or .obj/.ply models ✅
 - Skybox rendering (visible from anywhere, no ceiling) ✅
 - Two separate lighting/shader pipelines (one per room) ✅
 - Animated object in at least one room ✅
 - Mirror or transparency effect in one room ✅
 - Room-specific lighting (lights in one room do not affect the other) ✅ 
 - FPS camera with restricted vertical movement ✅
 - Collision detection for walls ✅

# Conditions (Assignment Constraints)

## General Scene:

 - Two rooms connected by a passage, as per the floor plan (strict geometry). ✅
 - Minimum two “exhibit” objects in each room. ❌
 - Skybox must be present (no ceilings). ✅

## Room 1:

 - Uses a more “realistic” illumination model (e.g., Phong). ✅
 - At least one textured object. ✅
 - At least two lights (one movable), ✅
 - Lights affecting only Room 1. ✅
 - Contains the animated object. ✅
 - One light can be switched on/off. ✅

## Room 2:

 - Uses a different illumination model (can be simpler). ❌
 - Must have at least one surface with transparency or mirror. ✅ (transparency, no mirror)
 - Lights affect only Room 2. ✅

## Bonus:

 - Shadows (main point light). ✅
 - Normal maps. ✅
 - Specular maps. ✅

**Map:**

---

The walls are named follwing their axis, Z'n' and X'n' are the walls in the Z and X axis respectively.

![Room Structure](assets/images/Room_Structure.png)

---