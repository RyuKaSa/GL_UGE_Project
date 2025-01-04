#version 330 core

// Maximum number of additional point lights
#define MAX_ADDITIONAL_LIGHTS 100

// Uniforms for additional point lights
uniform int uNumAdditionalLights;
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS];      
uniform vec3 uAdditionalLightColor[MAX_ADDITIONAL_LIGHTS];
uniform float uAdditionalLightIntensity[MAX_ADDITIONAL_LIGHTS];

// Input from vertex shader
in vec3 vNormal;
in vec3 vFragPos;         
in vec3 vFragPosWorld;    
in vec2 vTexCoords;
in mat3 TBN;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;           
uniform vec3 uKs;           
uniform float uShininess;   

// Transparency
uniform float uAlpha;

// Main light properties (in view space)
uniform vec3 uLightPos_vs;    
uniform vec3 uLightIntensity; 
uniform float farPlane;

// Camera position in world space
uniform vec3 cameraPosWorld;

// Texture samplers
uniform sampler2D uTexture;
uniform float uUseTexture;        

// Normal map
uniform sampler2D uNormalMap;
uniform float uUseNormalMap;

// Specular map
uniform sampler2D uSpecularMap;
uniform float uUseSpecularMap;

// Shadow mapping
uniform samplerCube depthMap;

// Light position in world space
uniform vec3 lightPosWorld;

// main output is main light intensity, no texture or anything else

// **Fragment Shader Main Function**
void main() {
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    // try kD light
    FragColor = vec4(uKd, 1.0);
}