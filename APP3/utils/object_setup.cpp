#include "object_setup.hpp"

// need parameters
// sphereVAO, sphereVertexCount, cubeVAO, cubeIndexCount
void setupSceneObjects(GLuint sphereVAO, GLuint sphereVertexCount, GLuint cubeVAO, GLuint cubeIndexCount) {

    glm::vec3 initialPosition(25.0f, 2.0f, 3.0f);
    glm::vec3 initialPosition2(25.0f, 2.0f, 4.0f);
    glm::vec3 initialSize(1.0f, 1.0f, 1.0f);

    // print debug
    std::cout << "Setting up scene objects..." << std::endl;

    utils_scene::addTransparentSphere(
        "whiteSphere",                // Name
        glm::vec3(30.0f, 5.0f, 4.0f), // Position
        1.0f,                         // Radius
        whiteMaterial,                // Material
        sphereVAO,                    // VAO ID
        sphereVertexCount,             // Index count
        true                          // Is static
    );

    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 floorSize(42.0f, 1.0f, 24.0f);
    utils_scene::createCompositeCube(
        "floor",             // Name
        origin,              // Origin
        floorSize,           // Size
        oak_planks_material, // Material
        cubeVAO,             // VAO ID
        cubeIndexCount,      // Index count
        true                 // Is static
    );

    // wall X1
    glm::vec3 wallPosition1(0.0f, 1.0f, -1.0f);
    glm::vec3 wallSizeX(42.0f, 3.0f, 1.0f);
    utils_scene::createCompositeCube(
        "wall_X1",      // Name
        wallPosition1,  // Position
        wallSizeX,      // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall X2
    glm::vec3 wallPosition2(0.0f, 1.0f, 24.0f);
    utils_scene::createCompositeCube(
        "wall_X2",      // Name
        wallPosition2,  // Position
        wallSizeX,      // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall Z1
    glm::vec3 wallPosition3(-1.0f, 1.0f, 0.0f);
    glm::vec3 wallSizeZ1(1.0f, 3.0f, 24.0f);
    utils_scene::createCompositeCube(
        "wall_Z1",      // Name
        wallPosition3,  // Position
        wallSizeZ1,     // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall Z2
    glm::vec3 wallPosition4(42.0f, 1.0f, 0.0f);
    utils_scene::createCompositeCube(
        "wall_Z2",      // Name
        wallPosition4,  // Position
        wallSizeZ1,     // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // separation wall, Z3
    glm::vec3 wallPosition5(20.0f, 1.0f, 0.0f);
    glm::vec3 wallSizeZ2(2.0f, 3.0f, 10.0f);
    utils_scene::createCompositeCube(
        "wall_Z3",          // Name
        wallPosition5,      // Position
        wallSizeZ2,         // Size
        terracottaMaterial, // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    // separation wall, Z4
    glm::vec3 wallPosition6(20.0f, 1.0f, 14.0f);
    utils_scene::createCompositeCube(
        "wall_Z4",          // Name
        wallPosition6,      // Position
        wallSizeZ2,         // Size
        terracottaMaterial, // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    // room 2 nether portal build, we can use composite cube for this, and make 4 composite cubes
    // first composite cube
    glm::vec3 portalPosition1(38.0f, 1.0f, 10.0f);
    glm::vec3 portalSize1(1.0f, 1.0f, 4.0f);

    utils_scene::createCompositeCube(
        "portal1",          // Name
        portalPosition1,    // Position
        portalSize1,        // Size
        obsidian_material,  // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    glm::vec3 portalPosition2(38.0f, 2.0f, 10.0f);
    glm::vec3 portalSize2(1.0f, 3.0f, 1.0f);

    utils_scene::createCompositeCube(
        "portal2",          // Name
        portalPosition2,    // Position
        portalSize2,        // Size
        obsidian_material,  // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    glm::vec3 portalPosition3(38.0f, 2.0f, 13.0f);

    utils_scene::createCompositeCube(
        "portal3",          // Name
        portalPosition3,    // Position
        portalSize2,        // Size
        obsidian_material,  // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    glm::vec3 portalPosition4(38.0f, 5.0f, 10.0f);
    glm::vec3 portalSize3(1.0f, 1.0f, 4.0f);

    utils_scene::createCompositeCube(
        "portal4",          // Name
        portalPosition4,    // Position
        portalSize3,        // Size
        obsidian_material,  // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    // now we an fill the portal with portal material
    glm::vec3 portalPosition5(38.0f, 2.0f, 11.0f);
    glm::vec3 portalSize4(1.0f, 3.0f, 2.0f);

    // Adding "portal" cube
    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 2.0f, 11.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 2.0f, 12.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );
    
    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 3.0f, 11.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 3.0f, 12.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 4.0f, 11.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addTransparentCube(
        "portal",                    // Name
        glm::vec3(38.0f, 4.0f, 12.0f), // Position
        glm::vec3(0.1f, 1.0f, 1.0f) * 0.999f, // Size
        portal_material,             // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // actual decor objects
    // decorcenterright (36, 2, 20)
    glm::vec3 decorCenterRight(33.0f, 1.0f, 20.0f);
    utils_scene::addCube(
        "30",          // Name
        decorCenterRight,  // Position
        initialSize,                 // Size
        crying_obsidian_material,    // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // decorecetnerriht + (1, 0, 0)
    utils_scene::addCube(
        "31",          // Name
        decorCenterRight + glm::vec3(1.0f, 0.0f, 0.0f),  // Position
        initialSize,                 // Size
        deepslate_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // decorecetnerriht + (0, 1, 0) emerald ore
    utils_scene::addCube(
        "32",          // Name
        decorCenterRight + glm::vec3(0.0f, 1.0f, 0.0f),  // Position
        initialSize,                 // Size
        deepslate_emerald_ore_material, // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // decorecetnerriht + (0, 0, 1) crying obsidian
    utils_scene::addCube(
        "33",          // Name
        decorCenterRight + glm::vec3(0.0f, 0.0f, 1.0f),  // Position
        initialSize,                 // Size
        crying_obsidian_material,    // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // decorecetnerriht + (-1, 0, -1) obsidian
    utils_scene::addCube(
        "34",          // Name
        decorCenterRight + glm::vec3(-1.0f, 0.0f, -1.0f),  // Position
        initialSize,                 // Size
        obsidian_material,           // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // decorecetnerriht + (-1, 0, 0) deepslate
    // decorecetnerriht + (-2, 0, 0) emerald ore
    // decorecetnerriht + (-3, 0, 1) crying obsidian
    // decorecetnerriht + (0, 0, 1) obsidian
    // decorecetnerriht + (-1, 0, 1) deepslate
    // decorecetnerriht + (-1, 0, -2) emerald ore
    // decorecetnerriht + (-1, 1, -2) crying obsidian
    // decorecetnerriht + (-1, 0, 1) obsidian
    // decorecetnerriht + (-1, 2, 2) deepslate
    // decorecetnerriht + (-1, 1, 2) emerald ore

    utils_scene::addCube(
        "35",          // Name
        decorCenterRight + glm::vec3(-1.0f, 0.0f, 0.0f),  // Position
        initialSize,                 // Size
        deepslate_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "36",          // Name
        decorCenterRight + glm::vec3(-2.0f, 0.0f, 0.0f),  // Position
        initialSize,                 // Size
        deepslate_emerald_ore_material, // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "37",          // Name
        decorCenterRight + glm::vec3(-3.0f, 0.0f, 1.0f),  // Position
        initialSize,                 // Size
        crying_obsidian_material,    // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "38",          // Name
        decorCenterRight + glm::vec3(0.0f, 0.0f, 1.0f),  // Position
        initialSize,                 // Size
        obsidian_material,           // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "39",          // Name
        decorCenterRight + glm::vec3(-1.0f, 0.0f, 1.0f),  // Position
        initialSize,                 // Size
        deepslate_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "40",          // Name
        decorCenterRight + glm::vec3(-1.0f, 0.0f, -2.0f),  // Position
        initialSize,                 // Size
        deepslate_emerald_ore_material, // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "41",          // Name
        decorCenterRight + glm::vec3(-1.0f, 1.0f, -2.0f),  // Position
        initialSize,                 // Size
        crying_obsidian_material,    // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "42",          // Name
        decorCenterRight + glm::vec3(-1.0f, 0.0f, 1.0f),  // Position
        initialSize,                 // Size
        obsidian_material,           // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "43",          // Name
        decorCenterRight + glm::vec3(-1.0f, 2.0f, 2.0f),  // Position
        initialSize,                 // Size
        deepslate_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    utils_scene::addCube(
        "44",          // Name
        decorCenterRight + glm::vec3(-1.0f, 1.0f, 2.0f),  // Position
        initialSize,                 // Size
        deepslate_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // Additional Blocks

    utils_scene::addCube(
        "45", decorCenterRight + glm::vec3(2.0f, 0.0f, 0.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "46", decorCenterRight + glm::vec3(-2.0f, 1.0f, 0.0f), initialSize,
        stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "47", decorCenterRight + glm::vec3(0.0f, 1.0f, -1.0f), initialSize,
        stoneMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "48", decorCenterRight + glm::vec3(1.0f, 0.0f, -1.0f), initialSize,
        wallMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "49", decorCenterRight + glm::vec3(-2.0f, 0.0f, -1.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "50", decorCenterRight + glm::vec3(1.0f, 1.0f, 1.0f), initialSize,
        stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "51", decorCenterRight + glm::vec3(-1.0f, 2.0f, -1.0f), initialSize,
        stoneMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "52", decorCenterRight + glm::vec3(0.0f, 2.0f, 0.0f), initialSize,
        wallMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "53", decorCenterRight + glm::vec3(-2.0f, 1.0f, 1.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "54", decorCenterRight + glm::vec3(2.0f, 1.0f, -1.0f), initialSize,
        stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "55", decorCenterRight + glm::vec3(0.0f, 0.0f, -2.0f), initialSize,
        stoneMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "56", decorCenterRight + glm::vec3(-1.0f, 1.0f, -3.0f), initialSize,
        wallMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "57", decorCenterRight + glm::vec3(1.0f, 2.0f, -2.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "58", decorCenterRight + glm::vec3(-2.0f, 2.0f, -1.0f), initialSize,
        stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    utils_scene::addCube(
        "59", decorCenterRight + glm::vec3(0.0f, 3.0f, 0.0f), initialSize,
        wallMaterial, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // left decor, (planks, iron block, note block, glass, purple stained glass)
    glm::vec3 decorCenterLeft(34.0f, 1.0f, 3.0f);

    // Block 1: Glass
    utils_scene::addTransparentCube(
        "60", decorCenterLeft, initialSize * 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 2: Purple Stained Glass
    utils_scene::addTransparentCube(
        "61", decorCenterLeft + glm::vec3(1.0f, 0.0f, 0.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 3: Iron Block
    utils_scene::addCube(
        "62", decorCenterLeft + glm::vec3(0.0f, 1.0f, 0.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 4: Note Block
    utils_scene::addCube(
        "63", decorCenterLeft + glm::vec3(0.0f, 0.0f, 1.0f), initialSize,
        note_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 5: Oak Planks
    utils_scene::addCube(
        "64", decorCenterLeft + glm::vec3(-1.0f, 0.0f, -1.0f), initialSize,
        oak_planks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 6: Mossy Stone Bricks
    utils_scene::addCube(
        "65", decorCenterLeft + glm::vec3(-1.0f, 0.0f, 0.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 7: Glass
    utils_scene::addTransparentCube(
        "66", decorCenterLeft + glm::vec3(-2.0f, 0.0f, 0.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 8: Purple Stained Glass
    utils_scene::addTransparentCube(
        "67", decorCenterLeft + glm::vec3(-3.0f, 0.0f, 1.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 9: Iron Block
    utils_scene::addCube(
        "68", decorCenterLeft + glm::vec3(0.0f, 0.0f, -1.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 10: Note Block
    utils_scene::addCube(
        "69", decorCenterLeft + glm::vec3(-1.0f, 0.0f, 1.0f), initialSize,
        note_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 11: Oak Planks
    utils_scene::addCube(
        "70", decorCenterLeft + glm::vec3(-1.0f, 1.0f, -2.0f), initialSize,
        oak_planks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 12: Mossy Stone Bricks
    utils_scene::addCube(
        "71", decorCenterLeft + glm::vec3(-1.0f, 2.0f, -2.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 13: Glass
    utils_scene::addTransparentCube(
        "72", decorCenterLeft + glm::vec3(-1.0f, 0.0f, 1.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 14: Purple Stained Glass
    utils_scene::addTransparentCube(
        "73", decorCenterLeft + glm::vec3(-1.0f, 2.0f, 2.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 15: Iron Block
    utils_scene::addCube(
        "74", decorCenterLeft + glm::vec3(-1.0f, 1.0f, 2.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Additional Blocks around decorCenterLeft

    // Block 16: Glass
    utils_scene::addTransparentCube(
        "75", decorCenterLeft + glm::vec3(1.0f, 1.0f, 0.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 17: Purple Stained Glass
    utils_scene::addTransparentCube(
        "76", decorCenterLeft + glm::vec3(0.0f, 1.0f, 1.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 18: Iron Block
    utils_scene::addCube(
        "77", decorCenterLeft + glm::vec3(2.0f, 0.0f, 0.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 19: Note Block
    utils_scene::addCube(
        "78", decorCenterLeft + glm::vec3(0.0f, 2.0f, 0.0f), initialSize,
        note_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 20: Oak Planks
    utils_scene::addCube(
        "79", decorCenterLeft + glm::vec3(-2.0f, 1.0f, 1.0f), initialSize,
        oak_planks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 21: Mossy Stone Bricks
    utils_scene::addCube(
        "80", decorCenterLeft + glm::vec3(-1.0f, 1.0f, 1.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 22: Glass
    utils_scene::addTransparentCube(
        "81", decorCenterLeft + glm::vec3(0.0f, 1.0f, -1.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 23: Purple Stained Glass
    utils_scene::addTransparentCube(
        "82", decorCenterLeft + glm::vec3(-1.0f, 2.0f, 0.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 24: Iron Block
    utils_scene::addCube(
        "83", decorCenterLeft + glm::vec3(1.0f, 0.0f, 1.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 25: Note Block
    utils_scene::addCube(
        "84", decorCenterLeft + glm::vec3(-2.0f, 0.0f, -1.0f), initialSize,
        note_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 26: Oak Planks
    utils_scene::addCube(
        "85", decorCenterLeft + glm::vec3(0.0f, 0.0f, -2.0f), initialSize,
        oak_planks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 27: Mossy Stone Bricks
    utils_scene::addCube(
        "86", decorCenterLeft + glm::vec3(-1.0f, 0.0f, 2.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 28: Glass
    utils_scene::addTransparentCube(
        "87", decorCenterLeft + glm::vec3(1.0f, 2.0f, -1.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 29: Purple Stained Glass
    utils_scene::addTransparentCube(
        "88", decorCenterLeft + glm::vec3(2.0f, 1.0f, 1.0f), initialSize* 0.999f,
        purple_stained_glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 30: Iron Block
    utils_scene::addCube(
        "89", decorCenterLeft + glm::vec3(0.0f, 2.0f, -2.0f), initialSize,
        iron_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 31: Note Block
    utils_scene::addCube(
        "90", decorCenterLeft + glm::vec3(-2.0f, 2.0f, 0.0f), initialSize,
        note_block_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 32: Oak Planks
    utils_scene::addCube(
        "91", decorCenterLeft + glm::vec3(1.0f, 1.0f, -2.0f), initialSize,
        oak_planks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 33: Mossy Stone Bricks
    utils_scene::addCube(
        "92", decorCenterLeft + glm::vec3(0.0f, 2.0f, 1.0f), initialSize,
        mossy_stone_bricks_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // Block 34: Glass
    utils_scene::addTransparentCube(
        "93", decorCenterLeft + glm::vec3(-2.0f, 1.0f, -2.0f), initialSize* 0.999f,
        glass_material, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
        cubeVAO, cubeIndexCount, true);

    // =======================

    // decor objects room 2

    // transparent cube in room 2
    // utils_scene::addTransparentCube(
    //     "transparent_cube",           // Name
    //     transparentCubePosition,      // Position
    //     transparentCubeSize,          // Size
    //     transparent_iron_block_material,          // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),  // Rotation axis (Y-axis)
    //     0.0f,                         // Rotation angle
    //     cubeVAO,                      // VAO ID
    //     cubeIndexCount,               // Index count
    //     true                          // Is static
    // );

    // second transparent cube in room 2
    // glm::vec3 transparentCubePosition2(32.0f, 2.0f, 12.0f);
    // utils_scene::addTransparentCube(
    //     "transparent_cube2",            // Name
    //     transparentCubePosition2,       // Position
    //     transparentCubeSize,            // Size
    //     purple_stained_glass_material, // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),    // Rotation axis (Y-axis)
    //     0.0f,                           // Rotation angle
    //     cubeVAO,                        // VAO ID
    //     cubeIndexCount,                 // Index count
    //     true                            // Is static
    // );

    // third transparent cube in room 2
    // glm::vec3 transparentCubePosition(32.0f, 2.0f, 10.0f);
    // glm::vec3 transparentCubePosition3(33.0f, 1.501f, 11.5f);
    // glm::vec3 transparentCubeSize(10.0f, 2.0f, 10.0f);
    // utils_scene::addTransparentCube(
    //     "transparent_cube3",            // Name
    //     transparentCubePosition3,       // Position
    //     transparentCubeSize,            // Size
    //     glass_material, // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),    // Rotation axis (Y-axis)
    //     0.0f,                           // Rotation angle
    //     cubeVAO,                        // VAO ID
    //     cubeIndexCount,                 // Index count
    //     true                            // Is static
    // );

    // glm::vec3 transparentCubePosition4(27.0f, 1.001f, 7.0f);
    // glm::vec3 transparentCubeSize(10.0f, 2.0f, 10.0f);
    // utils_scene::createTransparentCompositeCube(
    //     "glassthing",          // Name
    //     transparentCubePosition4,      // Position
    //     transparentCubeSize,         // Size
    //     glass_material, // Material
    //     cubeVAO,            // VAO ID
    //     cubeIndexCount,     // Index count
    //     true                // Is static
    // );

    // 

    // load a soccer ball as sophisticated object
    // utils_scene::addSphere(
    //     "soccer_ball",                // Name
    //     glm::vec3(27.0f, 3.0f, 10.0f), // Position
    //     0.3f,                         // Radius
    //     soccerMaterial,               // Material
    //     sphereVAO,                    // VAO ID
    //     sphereVertexCount,            // Vertex count
    //     true                          // Is static
    // );

    // load skybox sphere
    utils_scene::addSkySphere(
        "sky",                       // Name
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        99.0f,                       // Radius
        skyMaterial,                 // Material
        sphereVAO,                   // VAO ID
        sphereVertexCount,           // Vertex count
        false                        // Is static
    );

    // planet balls
    // positions:
    // sun: 9.5f, 3.0f, 11.5f (size1 = 1.3f) size2 = 1.2f size3 = 1.1f
    // emrcury: 8.5f, 3.0f, 11.5f
    // venus: 7.5f, 3.0f, 11.5f
    // earth: 5.0f, 3.0f, 12.0f (size1 = 0.3f) size2 = 0.34f
    // mars: 3.5f, 3.0f, 11.5f
    // jupiter: 3.0f, 3.0f, 11.5f
    // saturn: -0.5f, 3.0f, 11.5f
    // uranus: -2.5f, 3.0f, 11.5f
    // neptune: -4.5f, 3.0f, 11.5f


    // sun1
    utils_scene::addTransparentSphere(
        "sun",                      // Name
        glm::vec3(9.5f, 34.0f, 11.5f), // Position
        3.3f,                       // Radius
        sunMaterial,                // Material
        sphereVAO,                  // VAO ID
        sphereVertexCount,          // Vertex count
        true                        // Is static
    );

    // mercury
    utils_scene::addSphere(
        "mercury",                  // Name
        glm::vec3(9.5f, 3.0f, 3.5f), // Position
        0.3f,                      // Radius
        mercuryMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus
    utils_scene::addSphere(
        "venus",                  // Name
        glm::vec3(9.5f, 3.0f, 4.5f), // Position
        0.34f,                      // Radius
        venusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus atmosphere
    utils_scene::addTransparentSphere(
        "venus_atmosphere",                  // Name
        glm::vec3(10.5f, 3.0f, 4.5f), // Position
        0.36f,                      // Radius
        venusAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth
    utils_scene::addSphere(
        "earth",                  // Name
        glm::vec3(9.5f, 3.0f, 5.5f), // Position
        0.3f,                      // Radius
        earthMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth atmosphere
    utils_scene::addTransparentSphere(
        "earth_atmosphere",                  // Name
        glm::vec3(10.5f, 3.0f, 5.5f), // Position
        0.34f,                      // Radius
        earthAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // mars
    utils_scene::addSphere(
        "mars",                  // Name
        glm::vec3(9.5f, 3.0f, 6.5f), // Position
        0.3f,                      // Radius
        marsMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // jupiter
    utils_scene::addSphere(
        "jupiter",                  // Name
        glm::vec3(9.5f, 3.0f, 7.5f), // Position
        0.8f,                      // Radius
        jupiterMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn
    utils_scene::addSphere(
        "saturn",                  // Name
        glm::vec3(9.5f, 3.0f, 9.5f), // Position
        0.7f,                      // Radius
        saturnMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn ring
    utils_scene::addTransparentSphere(
        "saturn_ring",                  // Name
        glm::vec3(13.0f, 3.0f, 9.5f), // Position
        1.4f,                      // Radius
        saturnRingMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // uranus
    utils_scene::addSphere(
        "uranus",                  // Name
        glm::vec3(9.5f, 3.0f, 11.5f), // Position
        0.45f,                      // Radius
        uranusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // neptune
    utils_scene::addSphere(
        "neptune",                  // Name
        glm::vec3(9.5f, 3.0f, 13.5f), // Position
        0.42f,                      // Radius
        neptuneMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // display cases we will add a glass cube with a planet inside, for each planet

    // mercury
    utils_scene::addTransparentCube(
        "mercury_case",                 // Name
        glm::vec3(4.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // venus
    utils_scene::addTransparentCube(
        "venus_case",                 // Name
        glm::vec3(8.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // earth
    utils_scene::addTransparentCube(
        "earth_case",                 // Name
        glm::vec3(12.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // mars
    utils_scene::addTransparentCube(
        "mars_case",                 // Name
        glm::vec3(16.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // right side:
    // jupiter
    utils_scene::addTransparentCube(
        "jupiter_case",                 // Name
        glm::vec3(16.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // saturn
    utils_scene::addTransparentCube(
        "saturn_case",                 // Name
        glm::vec3(12.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // uranus
    utils_scene::addTransparentCube(
        "uranus_case",                 // Name
        glm::vec3(8.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // neptune
    utils_scene::addTransparentCube(
        "neptune_case",                 // Name
        glm::vec3(4.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // =======================
    // now we can load the planets that go in each case

    // mercury
    utils_scene::addSphere(
        "mercury_display",         // Name
        glm::vec3(4.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        mercuryMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus
    utils_scene::addSphere(
        "venus_display",         // Name
        glm::vec3(8.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        venusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus atmosphere
    utils_scene::addTransparentSphere(
        "venus_atmosphere_display",         // Name
        glm::vec3(8.0f, 1.5f, 3.0f), // Position
        0.62f,                      // Radius
        venusAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth
    utils_scene::addSphere(
        "earth_display",         // Name
        glm::vec3(12.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        earthMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth atmosphere
    utils_scene::addTransparentSphere(
        "earth_atmosphere_display",         // Name
        glm::vec3(12.0f, 1.5f, 3.0f), // Position
        0.62f,                      // Radius
        earthAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // mars
    utils_scene::addSphere(
        "mars_display",         // Name
        glm::vec3(16.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        marsMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // otehr side
    // jupiter
    utils_scene::addSphere(
        "jupiter_display",         // Name
        glm::vec3(16.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        jupiterMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn
    utils_scene::addSphere(
        "saturn_display",         // Name
        glm::vec3(12.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        saturnMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn ring
    utils_scene::addTransparentSphere(
        "saturn_ring_display",         // Name
        glm::vec3(12.0f, 1.5f, 20.0f), // Position
        1.1f,                      // Radius
        saturnRingMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // uranus
    utils_scene::addSphere(
        "uranus_display",         // Name
        glm::vec3(8.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        uranusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // neptune
    utils_scene::addSphere(
        "neptune_display",         // Name
        glm::vec3(4.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        neptuneMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );


}
