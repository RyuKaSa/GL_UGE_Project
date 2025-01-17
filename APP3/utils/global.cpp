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

bool isRockingChairPaused = false;
double rockingChairPausedTime = 0.0;
double rockingChairStartTime = 0.0;

bool isLightPaused = false;

float sensitivity = 0.1f;
float currentFrame = 0.0f;

int MAX_ADDITIONAL_LIGHTS = 100; // Maximum number of additional lights excluding the main point light which has shadows

bool wireframeMode = false;

// Define color masks for RGB channels
glm::vec3 colorMasks[3] = {
    glm::vec3(1.0f, 0.0f, 0.0f), // Red
    glm::vec3(0.0f, 1.0f, 0.0f), // Green
    glm::vec3(0.0f, 0.0f, 1.0f)  // Blue
};

float yaw = -90.0f; // Horizontal angle
float pitch = 0.0f; // Vertical angle

// Center of the screen
int centerX = window_width / 2;
int centerY = window_height / 2;
