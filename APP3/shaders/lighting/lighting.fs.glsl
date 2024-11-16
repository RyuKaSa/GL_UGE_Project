#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 viewPos;

uniform mat4 lightSpaceMatrix;

// Function to calculate shadow factor
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Transform fragment position to light space
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    // Sample shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // Shadow bias
    float bias = 0.005;
    float shadow = currentDepth > closestDepth + bias ? 0.5 : 1.0;

    // Discard fragments outside shadow map range
    if (projCoords.z > 1.0)
        shadow = 1.0;

    return shadow;
}

void main()
{
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec4 AlbedoSpec = texture(gAlbedoSpec, TexCoords);
    vec3 albedo = AlbedoSpec.rgb;
    float specular = AlbedoSpec.a;

    // Calculate ambient lighting
    vec3 ambient = 0.1 * albedo;

    // Calculate diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;

    // Calculate specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specularColor = specular * spec * lightColor;

    // Calculate shadow
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    // Combine lighting (with shadow effect)
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specularColor)) * lightIntensity;
    FragColor = vec4(lighting, 1.0);
}