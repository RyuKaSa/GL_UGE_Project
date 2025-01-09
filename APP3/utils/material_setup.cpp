#include "material_setup.hpp"

// Define the alpha value for transparency
float alphaOpaque = 1.0f; // Fully opaque
float alphaTransparent1 = 0.1f;
float alphaTransparent2 = 0.2f;
float alphaTransparent3 = 0.3f;
float alphaTransparent4 = 0.4f;
float alphaTransparent5 = 0.5f;
float alphaTransparent6 = 0.6f;
float alphaTransparent7 = 0.7f;
float alphaTransparent8 = 0.8f;
float alphaTransparent9 = 0.9f;

Material stoneMaterial;
Material wallMaterial;
Material terracottaMaterial;
Material transparentMaterialNoTexture;
Material transparentMaterialWithTexture;
Material rockingChairMaterial;
Material torusMaterial;
Material soccerMaterial;
Material stone_bricks_material;
Material glass_material;
Material purple_stained_glass_material;
Material iron_block_material;
Material note_block_material;
Material oak_planks_material;
Material deepslate_emerald_ore_material;
Material transparent_iron_block_material;
Material skyMaterial;
Material lightMaterial;
Material simpleLightMaterial;
Material sunMaterial;
Material mercuryMaterial;
Material venusMaterial;
Material venusAtmosphereMaterial;
Material earthMaterial;
Material earthAtmosphereMaterial;
Material marsMaterial;
Material jupiterMaterial;
Material saturnMaterial;
Material saturnRingMaterial;
Material uranusMaterial;
Material neptuneMaterial;
Material crying_obsidian_material;
Material deepslate_material;
Material mossy_stone_bricks_material;
Material obsidian_material;
Material portal_material;
Material whiteMaterial;

GLuint getTextureIDByName(const std::string& name, 
                          const std::vector<utils_loader::TextureInfo>& textures, 
                          const std::vector<GLuint>& selectedSkyboxTextures = {}) {
    for (const auto& texture : textures) {
        if (texture.name == name) {
            return *(texture.textureID);
        }
    }

    // If not found in regular textures, it should to be theskybox texture
    if (!selectedSkyboxTextures.empty() && name.find("Skybox") != std::string::npos) {
        return selectedSkyboxTextures[0];
    }

    throw std::runtime_error("Texture not found: " + name);
}


void createMaterials(MaterialManager& materialManager,
                     const std::vector<utils_loader::TextureInfo>& textures,
                     const std::vector<GLuint>& selectedSkyboxTextures) {

    stoneMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    stoneMaterial.hasDiffuseMap = true;
    stoneMaterial.diffuseMapID = getTextureIDByName(std::string("Stone"), textures, selectedSkyboxTextures);
    stoneMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    stoneMaterial.shininess = 32.0f;
    stoneMaterial.hasSpecularMap = false;
    stoneMaterial.hasNormalMap = true;
    stoneMaterial.normalMapID = getTextureIDByName(std::string("Stone Normal Map"), textures, selectedSkyboxTextures);
    stoneMaterial.alpha = alphaOpaque;
    materialManager.addOrGetMaterial(stoneMaterial);


    wallMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    wallMaterial.hasDiffuseMap = true;
    wallMaterial.diffuseMapID = getTextureIDByName(std::string("Cobblestone"), textures, selectedSkyboxTextures);
    wallMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    wallMaterial.shininess = 32.0f;
    wallMaterial.hasSpecularMap = false;
    wallMaterial.hasNormalMap = true;
    wallMaterial.normalMapID = getTextureIDByName(std::string("Cobblestone Normal Map"), textures, selectedSkyboxTextures);
    wallMaterial.alpha = alphaOpaque;
    materialManager.addOrGetMaterial(wallMaterial);

    terracottaMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    terracottaMaterial.hasDiffuseMap = true;
    terracottaMaterial.diffuseMapID = getTextureIDByName(std::string("Brown Terracotta"), textures, selectedSkyboxTextures);
    terracottaMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    terracottaMaterial.shininess = 32.0f;                // Shininess exponent
    terracottaMaterial.hasSpecularMap = false;
    terracottaMaterial.specularMapID = 0;
    terracottaMaterial.hasNormalMap = true;
    terracottaMaterial.normalMapID = getTextureIDByName(std::string("Brown Terracotta Normal Map"), textures, selectedSkyboxTextures);
    terracottaMaterial.alpha = alphaOpaque; // Opaque

    transparentMaterialNoTexture.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    transparentMaterialNoTexture.hasDiffuseMap = false;
    transparentMaterialNoTexture.diffuseMapID = 0;
    transparentMaterialNoTexture.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    transparentMaterialNoTexture.shininess = 32.0f;                // Shininess exponent
    transparentMaterialNoTexture.hasSpecularMap = false;
    transparentMaterialNoTexture.specularMapID = 0;
    transparentMaterialNoTexture.hasNormalMap = false;
    transparentMaterialNoTexture.normalMapID = 0;
    transparentMaterialNoTexture.alpha = alphaTransparent1; // 30% opaque

    transparentMaterialWithTexture.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    transparentMaterialWithTexture.hasDiffuseMap = true;
    transparentMaterialWithTexture.diffuseMapID = getTextureIDByName(std::string("Cobblestone"), textures, selectedSkyboxTextures);
    transparentMaterialWithTexture.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    transparentMaterialWithTexture.shininess = 32.0f;                // Shininess exponent
    transparentMaterialWithTexture.hasSpecularMap = false;           // no specular map
    transparentMaterialWithTexture.specularMapID = 0;
    transparentMaterialWithTexture.hasNormalMap = true;
    transparentMaterialWithTexture.normalMapID = getTextureIDByName(std::string("Cobblestone Normal Map"), textures, selectedSkyboxTextures);
    transparentMaterialWithTexture.alpha = alphaTransparent2;

    rockingChairMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    rockingChairMaterial.hasDiffuseMap = true;
    rockingChairMaterial.diffuseMapID = getTextureIDByName(std::string("Chair Base Color"), textures, selectedSkyboxTextures);
    rockingChairMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    rockingChairMaterial.shininess = 32.0f;                // Shininess exponent
    rockingChairMaterial.hasSpecularMap = false;           // no specular map
    rockingChairMaterial.specularMapID = 0;
    rockingChairMaterial.hasNormalMap = true;
    rockingChairMaterial.normalMapID = getTextureIDByName(std::string("Chair Normal Map"), textures, selectedSkyboxTextures);
    rockingChairMaterial.alpha = alphaOpaque; // Opaque

    torusMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    torusMaterial.hasDiffuseMap = false;
    torusMaterial.diffuseMapID = 0;
    torusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    torusMaterial.shininess = 32.0f;                // Shininess exponent
    torusMaterial.hasSpecularMap = false;
    torusMaterial.specularMapID = 0;
    torusMaterial.hasNormalMap = false;
    torusMaterial.normalMapID = 0;
    torusMaterial.alpha = alphaTransparent5; // 50% opaque

    soccerMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    soccerMaterial.hasDiffuseMap = true;
    soccerMaterial.diffuseMapID = getTextureIDByName(std::string("Soccer Ball"), textures, selectedSkyboxTextures);
    soccerMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    soccerMaterial.shininess = 32.0f;                // Shininess exponent
    soccerMaterial.hasSpecularMap = false;           // no specular map
    soccerMaterial.specularMapID = 0;
    soccerMaterial.hasNormalMap = true;
    soccerMaterial.normalMapID = getTextureIDByName(std::string("Soccer Ball Normal Map"), textures, selectedSkyboxTextures);
    soccerMaterial.alpha = alphaOpaque;

    stone_bricks_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    stone_bricks_material.hasDiffuseMap = true;
    stone_bricks_material.diffuseMapID = getTextureIDByName(std::string("Stone Bricks"), textures, selectedSkyboxTextures);
    stone_bricks_material.Ks = glm::vec3(0.4f, 0.4f, 0.4f);
    stone_bricks_material.shininess = 16.0f;
    stone_bricks_material.hasSpecularMap = true;
    stone_bricks_material.specularMapID = getTextureIDByName(std::string("Stone Bricks Specular Map"), textures, selectedSkyboxTextures);
    stone_bricks_material.hasNormalMap = true;
    stone_bricks_material.normalMapID = getTextureIDByName(std::string("Stone Bricks Normal Map"), textures, selectedSkyboxTextures);
    stone_bricks_material.alpha = alphaOpaque;

    glass_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    glass_material.hasDiffuseMap = true;
    glass_material.diffuseMapID = getTextureIDByName(std::string("Glass"), textures, selectedSkyboxTextures);
    glass_material.Ks = glm::vec3(0.9f, 0.9f, 0.9f);
    glass_material.shininess = 130.0f;
    glass_material.hasSpecularMap = false;
    glass_material.specularMapID = 0;
    glass_material.hasNormalMap = false;
    glass_material.normalMapID = 0;
    glass_material.alpha = alphaTransparent5;

    purple_stained_glass_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    purple_stained_glass_material.hasDiffuseMap = true;
    purple_stained_glass_material.diffuseMapID = getTextureIDByName(std::string("Purple Stained Glass"), textures, selectedSkyboxTextures);
    purple_stained_glass_material.Ks = glm::vec3(0.9f, 0.7f, 0.9f);
    purple_stained_glass_material.shininess = 130.0f;
    purple_stained_glass_material.hasSpecularMap = false;
    purple_stained_glass_material.specularMapID = 0;
    purple_stained_glass_material.hasNormalMap = false;
    purple_stained_glass_material.normalMapID = 0;
    purple_stained_glass_material.alpha = alphaTransparent5;

    iron_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    iron_block_material.hasDiffuseMap = true;
    iron_block_material.diffuseMapID = getTextureIDByName(std::string("Iron Block"), textures, selectedSkyboxTextures);
    iron_block_material.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
    iron_block_material.shininess = 64.0f;
    iron_block_material.hasSpecularMap = true;
    iron_block_material.specularMapID = getTextureIDByName(std::string("Iron Block Specular Map"), textures, selectedSkyboxTextures);
    iron_block_material.hasNormalMap = true;
    iron_block_material.normalMapID = getTextureIDByName(std::string("Iron Block Normal Map"), textures, selectedSkyboxTextures);
    iron_block_material.alpha = alphaOpaque;

    note_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    note_block_material.hasDiffuseMap = true;
    note_block_material.diffuseMapID = getTextureIDByName(std::string("Note Block"), textures, selectedSkyboxTextures);
    note_block_material.Ks = glm::vec3(0.2f, 0.2f, 0.2f);
    note_block_material.shininess = 8.0f;
    note_block_material.hasSpecularMap = true;
    note_block_material.specularMapID = getTextureIDByName(std::string("Note Block Specular Map"), textures, selectedSkyboxTextures);
    note_block_material.hasNormalMap = true;
    note_block_material.normalMapID = getTextureIDByName(std::string("Note Block Normal Map"), textures, selectedSkyboxTextures);
    note_block_material.alpha = alphaOpaque;

    oak_planks_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    oak_planks_material.hasDiffuseMap = true;
    oak_planks_material.diffuseMapID = getTextureIDByName(std::string("Oak Planks"), textures, selectedSkyboxTextures);
    oak_planks_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    oak_planks_material.shininess = 12.0f;
    oak_planks_material.hasSpecularMap = true;
    oak_planks_material.specularMapID = getTextureIDByName(std::string("Oak Planks Specular Map"), textures, selectedSkyboxTextures);
    oak_planks_material.hasNormalMap = true;
    oak_planks_material.normalMapID = getTextureIDByName(std::string("Oak Planks Normal Map"), textures, selectedSkyboxTextures);
    oak_planks_material.alpha = alphaOpaque;

    deepslate_emerald_ore_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    deepslate_emerald_ore_material.hasDiffuseMap = true;
    deepslate_emerald_ore_material.diffuseMapID = getTextureIDByName(std::string("Deepslate Emerald Ore"), textures, selectedSkyboxTextures);
    deepslate_emerald_ore_material.Ks = glm::vec3(0.6f, 0.8f, 0.6f);
    deepslate_emerald_ore_material.shininess = 54.0f;
    deepslate_emerald_ore_material.hasSpecularMap = true;
    deepslate_emerald_ore_material.specularMapID = getTextureIDByName(std::string("Deepslate Emerald Ore Specular Map"), textures, selectedSkyboxTextures);
    deepslate_emerald_ore_material.hasNormalMap = true;
    deepslate_emerald_ore_material.normalMapID = getTextureIDByName(std::string("Deepslate Emerald Ore Normal Map"), textures, selectedSkyboxTextures);
    deepslate_emerald_ore_material.alpha = alphaOpaque;

    transparent_iron_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    transparent_iron_block_material.hasDiffuseMap = true;
    transparent_iron_block_material.diffuseMapID = getTextureIDByName(std::string("Iron Block"), textures, selectedSkyboxTextures);
    transparent_iron_block_material.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
    transparent_iron_block_material.shininess = 64.0f;
    transparent_iron_block_material.hasSpecularMap = true;
    transparent_iron_block_material.specularMapID = getTextureIDByName(std::string("Iron Block Specular Map"), textures, selectedSkyboxTextures);
    transparent_iron_block_material.hasNormalMap = true;
    transparent_iron_block_material.normalMapID = getTextureIDByName(std::string("Iron Block Normal Map"), textures, selectedSkyboxTextures);
    transparent_iron_block_material.alpha = alphaTransparent5;

    skyMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    skyMaterial.hasDiffuseMap = true;
    skyMaterial.diffuseMapID = getTextureIDByName(std::string("Skybox"), textures, selectedSkyboxTextures);
    skyMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    skyMaterial.shininess = 0.0f;                 // Shininess exponent
    skyMaterial.hasSpecularMap = false;           //  no specular map
    skyMaterial.specularMapID = 0;
    skyMaterial.hasNormalMap = false;
    skyMaterial.normalMapID = 0;
    skyMaterial.alpha = alphaOpaque; // Opaque

    lightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    lightMaterial.hasDiffuseMap = false;
    lightMaterial.diffuseMapID = 0;
    lightMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    lightMaterial.shininess = 32.0f;                // Shininess exponent
    lightMaterial.hasSpecularMap = false;
    lightMaterial.specularMapID = 0;
    lightMaterial.hasNormalMap = false;
    lightMaterial.normalMapID = 0;
    lightMaterial.alpha = alphaOpaque; // Opaque

    simpleLightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // Default white color
    simpleLightMaterial.Ks = glm::vec3(0.0f);            // No specular reflection
    simpleLightMaterial.shininess = 1.0f;               // Low shininess
    simpleLightMaterial.alpha = 1.0f;     

    // sun material
    sunMaterial.Kd = glm::vec3(1.0f, 0.8f, 0.6f);
    sunMaterial.hasDiffuseMap = true;
    sunMaterial.diffuseMapID = getTextureIDByName(std::string("Sun"), textures, selectedSkyboxTextures);
    sunMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    sunMaterial.shininess = 0.0f;                // Shininess exponent
    sunMaterial.hasSpecularMap = false;
    sunMaterial.specularMapID = 0;
    sunMaterial.hasNormalMap = false;
    sunMaterial.normalMapID = 0;
    sunMaterial.alpha = alphaTransparent9; // Transparent

    mercuryMaterial.Kd = glm::vec3(0.5f, 0.5f, 0.5f);
    mercuryMaterial.hasDiffuseMap = true;
    mercuryMaterial.diffuseMapID = getTextureIDByName(std::string("Mercury"), textures, selectedSkyboxTextures);
    mercuryMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    mercuryMaterial.shininess = 20.0f;                // Shininess exponent
    mercuryMaterial.hasSpecularMap = false;
    mercuryMaterial.specularMapID = 0;
    mercuryMaterial.hasNormalMap = false;
    mercuryMaterial.normalMapID = 0;
    mercuryMaterial.alpha = alphaOpaque; // Opaque

    venusMaterial.Kd = glm::vec3(0.8f, 0.7f, 0.6f);
    venusMaterial.hasDiffuseMap = true;
    venusMaterial.diffuseMapID = getTextureIDByName(std::string("Venus Surface"), textures, selectedSkyboxTextures);
    venusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    venusMaterial.shininess = 25.0f;                // Shininess exponent
    venusMaterial.hasSpecularMap = false;
    venusMaterial.specularMapID = 0;
    venusMaterial.hasNormalMap = false;
    venusMaterial.normalMapID = 0;
    venusMaterial.alpha = alphaOpaque; // Opaque

    venusAtmosphereMaterial.Kd = glm::vec3(0.7f, 0.7f, 0.9f);
    venusAtmosphereMaterial.hasDiffuseMap = true;
    venusAtmosphereMaterial.diffuseMapID = getTextureIDByName(std::string("Venus Atmosphere"), textures, selectedSkyboxTextures);
    venusAtmosphereMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    venusAtmosphereMaterial.shininess = 5.0f;                // Shininess exponent
    venusAtmosphereMaterial.hasSpecularMap = false;
    venusAtmosphereMaterial.specularMapID = 0;
    venusAtmosphereMaterial.hasNormalMap = false;
    venusAtmosphereMaterial.normalMapID = 0;
    venusAtmosphereMaterial.alpha = alphaTransparent3; // Transparent

    earthMaterial.Kd = glm::vec3(0.4f, 0.6f, 0.8f);
    earthMaterial.hasDiffuseMap = true;
    earthMaterial.diffuseMapID = getTextureIDByName(std::string("Earth"), textures, selectedSkyboxTextures);
    earthMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    earthMaterial.shininess = 30.0f;                // Shininess exponent
    earthMaterial.hasSpecularMap = true;
    earthMaterial.specularMapID = getTextureIDByName(std::string("Earth Specular Map"), textures, selectedSkyboxTextures);
    earthMaterial.hasNormalMap = true;
    earthMaterial.normalMapID = getTextureIDByName(std::string("Earth Normal Map"), textures, selectedSkyboxTextures);
    earthMaterial.alpha = alphaOpaque; // Opaque

    earthAtmosphereMaterial.Kd = glm::vec3(0.6f, 0.7f, 0.9f);
    earthAtmosphereMaterial.hasDiffuseMap = true;
    earthAtmosphereMaterial.diffuseMapID = getTextureIDByName(std::string("Earth Atmosphere"), textures, selectedSkyboxTextures);
    earthAtmosphereMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    earthAtmosphereMaterial.shininess = 8.0f;                // Shininess exponent
    earthAtmosphereMaterial.hasSpecularMap = false;
    earthAtmosphereMaterial.specularMapID = 0;
    earthAtmosphereMaterial.hasNormalMap = false;
    earthAtmosphereMaterial.normalMapID = 0;
    earthAtmosphereMaterial.alpha = alphaTransparent8;

    // mars material
    marsMaterial.Kd = glm::vec3(0.6f, 0.3f, 0.2f);
    marsMaterial.hasDiffuseMap = true;
    marsMaterial.diffuseMapID = getTextureIDByName(std::string("Mars"), textures, selectedSkyboxTextures);
    marsMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    marsMaterial.shininess = 15.0f;                // Shininess exponent
    marsMaterial.hasSpecularMap = false;
    marsMaterial.specularMapID = 0;
    marsMaterial.hasNormalMap = false;
    marsMaterial.normalMapID = 0;
    marsMaterial.alpha = alphaOpaque; // Opaque

    // jupiter material
    jupiterMaterial.Kd = glm::vec3(0.7f, 0.6f, 0.5f);
    jupiterMaterial.hasDiffuseMap = true;
    jupiterMaterial.diffuseMapID = getTextureIDByName(std::string("Jupiter"), textures, selectedSkyboxTextures);
    jupiterMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    jupiterMaterial.shininess = 18.0f;                // Shininess exponent
    jupiterMaterial.hasSpecularMap = false;
    jupiterMaterial.specularMapID = 0;
    jupiterMaterial.hasNormalMap = false;
    jupiterMaterial.normalMapID = 0;
    jupiterMaterial.alpha = alphaOpaque; // Opaque

    // saturn material
    saturnMaterial.Kd = glm::vec3(0.8f, 0.7f, 0.6f);
    saturnMaterial.hasDiffuseMap = true;
    saturnMaterial.diffuseMapID = getTextureIDByName(std::string("Saturn"), textures, selectedSkyboxTextures);
    saturnMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    saturnMaterial.shininess = 25.0f;                // Shininess exponent
    saturnMaterial.hasSpecularMap = false;
    saturnMaterial.specularMapID = 0;
    saturnMaterial.hasNormalMap = false;
    saturnMaterial.normalMapID = 0;
    saturnMaterial.alpha = alphaOpaque; // Opaque

    // saturn ring material
    saturnRingMaterial.Kd = glm::vec3(0.9f, 0.8f, 0.7f);
    saturnRingMaterial.hasDiffuseMap = true;
    saturnRingMaterial.diffuseMapID = getTextureIDByName(std::string("Saturn Ring"), textures, selectedSkyboxTextures);
    saturnRingMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    saturnRingMaterial.shininess = 60.0f;                // Shininess exponent
    saturnRingMaterial.hasSpecularMap = false;
    saturnRingMaterial.specularMapID = 0;
    saturnRingMaterial.hasNormalMap = false;
    saturnRingMaterial.normalMapID = 0;
    saturnRingMaterial.alpha = alphaTransparent4; // Transparent

    // uranus material
    uranusMaterial.Kd = glm::vec3(0.5f, 0.7f, 0.8f);
    uranusMaterial.hasDiffuseMap = true;
    uranusMaterial.diffuseMapID = getTextureIDByName(std::string("Uranus"), textures, selectedSkyboxTextures);
    uranusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    uranusMaterial.shininess = 20.0f;                // Shininess exponent
    uranusMaterial.hasSpecularMap = false;
    uranusMaterial.specularMapID = 0;
    uranusMaterial.hasNormalMap = false;
    uranusMaterial.normalMapID = 0;
    uranusMaterial.alpha = alphaOpaque; // Opaque

    // neptune material
    neptuneMaterial.Kd = glm::vec3(0.4f, 0.6f, 0.9f);
    neptuneMaterial.hasDiffuseMap = true;
    neptuneMaterial.diffuseMapID = getTextureIDByName(std::string("Neptune"), textures, selectedSkyboxTextures);
    neptuneMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    neptuneMaterial.shininess = 22.0f;                // Shininess exponent
    neptuneMaterial.hasSpecularMap = false;
    neptuneMaterial.specularMapID = 0;
    neptuneMaterial.hasNormalMap = false;
    neptuneMaterial.normalMapID = 0;
    neptuneMaterial.alpha = alphaOpaque; // Opaque

    // crying obsidian
    crying_obsidian_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    crying_obsidian_material.hasDiffuseMap = true;
    crying_obsidian_material.diffuseMapID = getTextureIDByName(std::string("Crying Obsidian"), textures, selectedSkyboxTextures);
    crying_obsidian_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    crying_obsidian_material.shininess = 32.0f;                // Shininess exponent
    crying_obsidian_material.hasSpecularMap = true;
    crying_obsidian_material.specularMapID = getTextureIDByName(std::string("Crying Obsidian Specular Map"), textures, selectedSkyboxTextures);
    crying_obsidian_material.hasNormalMap = true;
    crying_obsidian_material.normalMapID = getTextureIDByName(std::string("Crying Obsidian Normal Map"), textures, selectedSkyboxTextures);
    crying_obsidian_material.alpha = alphaOpaque; // Opaque

    // deepslate
    deepslate_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    deepslate_material.hasDiffuseMap = true;
    deepslate_material.diffuseMapID = getTextureIDByName(std::string("Deepslate"), textures, selectedSkyboxTextures);
    deepslate_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    deepslate_material.shininess = 32.0f;                // Shininess exponent
    deepslate_material.hasSpecularMap = true;
    deepslate_material.specularMapID = getTextureIDByName(std::string("Deepslate Specular Map"), textures, selectedSkyboxTextures);
    deepslate_material.hasNormalMap = true;
    deepslate_material.normalMapID = getTextureIDByName(std::string("Deepslate Normal Map"), textures, selectedSkyboxTextures);
    deepslate_material.alpha = alphaOpaque; // Opaque

    // mossy stone bricks
    mossy_stone_bricks_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    mossy_stone_bricks_material.hasDiffuseMap = true;
    mossy_stone_bricks_material.diffuseMapID = getTextureIDByName(std::string("Mossy Stone Bricks"), textures, selectedSkyboxTextures);
    mossy_stone_bricks_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    mossy_stone_bricks_material.shininess = 32.0f;                // Shininess exponent
    mossy_stone_bricks_material.hasSpecularMap = true;
    mossy_stone_bricks_material.specularMapID = getTextureIDByName(std::string("Mossy Stone Bricks Specular Map"), textures, selectedSkyboxTextures);
    mossy_stone_bricks_material.hasNormalMap = true;
    mossy_stone_bricks_material.normalMapID = getTextureIDByName(std::string("Mossy Stone Bricks Normal Map"), textures, selectedSkyboxTextures);
    mossy_stone_bricks_material.alpha = alphaOpaque; // Opaque

    // obsidian
    obsidian_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    obsidian_material.hasDiffuseMap = true;
    obsidian_material.diffuseMapID = getTextureIDByName(std::string("Obsidian"), textures, selectedSkyboxTextures);
    obsidian_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    obsidian_material.shininess = 32.0f;                // Shininess exponent
    obsidian_material.hasSpecularMap = true;
    obsidian_material.specularMapID = getTextureIDByName(std::string("Obsidian Specular Map"), textures, selectedSkyboxTextures);
    obsidian_material.hasNormalMap = true;
    obsidian_material.normalMapID = getTextureIDByName(std::string("Obsidian Normal Map"), textures, selectedSkyboxTextures);
    obsidian_material.alpha = alphaOpaque; // Opaque

    // portal
    portal_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    portal_material.hasDiffuseMap = true;
    portal_material.diffuseMapID = getTextureIDByName(std::string("Portal"), textures, selectedSkyboxTextures);
    portal_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    portal_material.shininess = 32.0f;                // Shininess exponent
    portal_material.hasSpecularMap = true;
    portal_material.specularMapID = getTextureIDByName(std::string("Portal Specular Map"), textures, selectedSkyboxTextures);
    portal_material.hasNormalMap = false;
    portal_material.normalMapID = 0;
    portal_material.alpha = alphaTransparent5; // 50% opaque

    whiteMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    whiteMaterial.hasDiffuseMap = false;
    whiteMaterial.diffuseMapID = 0;
    whiteMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    whiteMaterial.shininess = 32.0f;                // Shininess exponent
    whiteMaterial.hasSpecularMap = false;           //  no specular map
    whiteMaterial.specularMapID = 0;
    whiteMaterial.hasNormalMap = false;
    whiteMaterial.normalMapID = 0;
    whiteMaterial.alpha = alphaTransparent1; // Transparent

    std::cout << "Materials initialized and added to the MaterialManager.\n";
    std::cout << "Number of materials: " << materialManager.getNumberOfMaterials() << std::endl;    
}
