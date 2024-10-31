#version 330 core
in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform samplerCube uShadowMap;
uniform vec3 uObjectColor;
uniform float uUseTexture;
uniform vec3 uLightPos;
uniform float uFarPlane;
uniform float uMinBias;
uniform float uMaxBias;
uniform float uLightIntensity;

float ShadowCalculation()
{
    vec3 fragToLight = vFragPos - uLightPos;
    float closestDepth = texture(uShadowMap, fragToLight).r * uFarPlane;
    float currentDepth = length(fragToLight);
    
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-fragToLight);
    float bias = max(uMaxBias * (1.0 - dot(normal, lightDir)), uMinBias);
    float shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    vec3 textureColor = texture(uTexture, vTexCoords).rgb;
    vec3 color = mix(uObjectColor, textureColor, uUseTexture);
    
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color * uLightIntensity;
    
    float ambient = 0.1;
    float shadow = ShadowCalculation();
    vec3 finalColor = (ambient + (1.0 - shadow) * diffuse) * color;
    
    FragColor = vec4(finalColor, 1.0);
}