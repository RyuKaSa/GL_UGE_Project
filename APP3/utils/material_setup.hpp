// #pragma once
// material_manager.hpp
#ifndef MATERIAL_SETUP_HPP
#define MATERIAL_SETUP_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "material.hpp"
#include "material_manager.hpp"

#include "resource_loader.hpp"

using namespace utils_loader;

extern Material stoneMaterial;
extern Material wallMaterial;
extern Material terracottaMaterial;
extern Material transparentMaterialNoTexture;
extern Material transparentMaterialWithTexture;
extern Material rockingChairMaterial;
extern Material torusMaterial;
extern Material soccerMaterial;
extern Material stone_bricks_material;
extern Material glass_material;
extern Material purple_stained_glass_material;
extern Material iron_block_material;
extern Material note_block_material;
extern Material oak_planks_material;
extern Material deepslate_emerald_ore_material;
extern Material transparent_iron_block_material;
extern Material skyMaterial;
extern Material lightMaterial;
extern Material simpleLightMaterial;
extern Material sunMaterial;
extern Material mercuryMaterial;
extern Material venusMaterial;
extern Material venusAtmosphereMaterial;
extern Material earthMaterial;
extern Material earthAtmosphereMaterial;
extern Material marsMaterial;
extern Material jupiterMaterial;
extern Material saturnMaterial;
extern Material saturnRingMaterial;
extern Material uranusMaterial;
extern Material neptuneMaterial;
extern Material crying_obsidian_material;
extern Material deepslate_material;
extern Material mossy_stone_bricks_material;
extern Material obsidian_material;
extern Material portal_material;
extern Material whiteMaterial;

// Helper function to get texture ID by name
// GLuint getTextureIDByName(const std::string name, const std::vector<utils_loader::TextureInfo> textures);

// Function to create materials
void createMaterials(MaterialManager& materialManager,
                     const std::vector<utils_loader::TextureInfo>& textures,
                     const std::vector<GLuint>& selectedSkyboxTextures);

#endif // MATERIAL_SETUP_HPP