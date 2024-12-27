

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

## note

If i implemente a big thing, i can try to test it in APP2, to keep APP1 working on previous version.

# How to run the thing

```bash
mkdir bin; cd bin; cmake ..; make -j4; ./main_executable
```

assignment : 

---

# Features to Implement

 - C++ application with OpenGL 3+ (shaders, no fixed pipeline)
 - Loading/importing of geometry or .obj/.ply models
 - Skybox rendering (visible from anywhere, no ceiling)
 - Two separate lighting/shader pipelines (one per room)
 - Animated object in at least one room
 - Mirror or transparency effect in one room
 - Room-specific lighting (lights in one room do not affect the other)
 - FPS camera with restricted vertical movement
 - Collision detection for walls

# Conditions (Assignment Constraints)

## General Scene:

 - Two rooms connected by a passage, as per the floor plan (strict geometry).
 - Minimum two “exhibit” objects in each room.
 - Skybox must be present (no ceilings).
 - Must compile on Linux (no memory leaks, documented, uses Git).

## Room 1:

 - Uses a more “realistic” illumination model (e.g., Phong).
 - At least one textured object.
 - At least two lights (one movable), affecting only Room 1.
 - Contains the animated object.

## Room 2:

 - Uses a different illumination model (can be simpler).
 - Must have at least one surface with transparency or mirror.
 - Lights (if any) affect only Room 2.


**Map:**


---

The walls are named follwing their axis, Z'n' and X'n' are the walls in the Z and X axis respectively.

![Room Structure](assets/images/Room_Structure.png)

---