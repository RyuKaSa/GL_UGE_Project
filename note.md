

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

**General:**
- Develop a 3D real-time application in C++ using OpenGL 3+ (no fixed pipeline, use shaders). *Done*
- Ensure the application runs on Linux and compiles with `g++` (tested on faculty machines). *I work on MacOS*
- Handle window management and events. *Done*
- Prevent memory leaks (use Valgrind for verification).
- Maintain clean, well-commented code. *Done*
- Use Git for version control and provide a `README.md` with build and usage instructions. *On github*
- Implement user navigation in FPS mode with fixed height, including wall collision management. *Done*
- Create a skybox visible from both rooms.
- Implement functionality to:
  - Exit the application via a keypress. *Done*
  - Toggle 'wireframe' mode on/off with a keypress, using glPolygon-Mode
  - Pause and restart the animation in room 1 via a keypress.
  - Turn a light in room 1 on/off via a keypress.

**Room 1:**
- Create at least two objects (including one original or imported `.obj`/`.ply` model).
- Animate one object with a complex motion (beyond simple rotation).
- Apply textures to at least one object. *Done*
- Implement at least two light sources with a complex illumination model (e.g., Phong shading).
- Ensure one light source is mobile, and its movement influences the illumination coherently. *Done*
- Ensure lighting in room 1 does not influence room 2.

**Room 2:**
- Create at least two objects (including one original or imported `.obj`/`.ply` model).
- Implement an illumination model distinct from room 1 (can be simple or more elaborate).
- Create a surface with transparency or a mirror effect (transparency with single/multiple layers or a mirror using advanced OpenGL techniques).
- Ensure lighting in room 2 does not influence room 1.

**Map:**


---

The walls are named follwing their axis, Z'n' and X'n' are the walls in the Z and X axis respectively.

![Room Structure](assets/images/Room_Structure.png)

---