

# 10-Nov-2024
currently trying to separate the main file into several more digestable parts, for ease of maintenance and readability.
Otherwise, the direcitonal light + shadows work, and the point light + shadows work.
The textures and normal maps work, and there are python converters for 8bit and normal map PNGs.

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