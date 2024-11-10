#include "global.hpp"

// Window dimensions
int window_width = 1600;
int window_height = 1000;

// Stone floor coordinates
int numCubesX = 15;    // Number of cubes along the x-axis
int numCubesZ = 15;    // Number of cubes along the z-axis
float spacingX = 1.0f; // Distance between each cube along the x-axis
float spacingZ = 1.0f; // Distance between each cube along the z-axis

const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

float cameraRadius = 0.15f; // Radius of the camera sphere for collision detection
float cameraHeight = 2.0f;  // Height of the camera cylinder