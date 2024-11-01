# Creating and Transforming Cubes in OpenGL

How to create new cubes with the current setup, apply transformations like scaling and rotation, and position them as needed. We'll use the methods provided in the code to generate cubes and manipulate their transformations.

## Table of Contents

1. [Creating a Cube](#creating-a-cube)
2. [Setting Up Cube Buffers](#setting-up-cube-buffers)
3. [Transforming Cubes](#transforming-cubes)
   - [Translation](#translation)
   - [Scaling](#scaling)
   - [Rotation](#rotation)
4. [Drawing the Cube](#drawing-the-cube)
5. [Examples](#examples)
   - [Example 1: Positioning a Cube Above the Sphere](#example-1-positioning-a-cube-above-the-sphere)
   - [Example 2: Scaling a Cube on Different Axes](#example-2-scaling-a-cube-on-different-axes)
   - [Example 3: Rotating a Cube Around the Y-Axis](#example-3-rotating-a-cube-around-the-y-axis)
6. [Additional Notes](#additional-notes)

---

## Creating a Cube

To create a cube in scene, we need to generate its vertices and indices. The `createCube` function handles this:

```cpp
void createCube(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indices);
```

- **Vertices**: An array of `Vertex3D` structures containing position, normal, and texture coordinates for each vertex.
- **Indices**: An array of indices that define how vertices are connected to form triangles.

## Setting Up Cube Buffers

Once we have the vertices and indices, set up the Vertex Buffer Object (VBO), Element Buffer Object (EBO), and Vertex Array Object (VAO) using the `setupCubeBuffers` function:

```cpp
void setupCubeBuffers(const std::vector<Vertex3D>& vertices, const std::vector<GLuint>& indices, GLuint& cubeVBO, GLuint& cubeEBO, GLuint& cubeVAO);
```

- **VBO**: Stores the vertex data.
- **EBO**: Stores the indices defining the cube's faces.
- **VAO**: Encapsulates the vertex attribute configuration.

## Transforming Cubes

Transformations allow us to position, scale, and rotate our cube within the scene. This is achieved by modifying the Model Matrix of the cube.

### Translation

To move the cube to a different position:

```cpp
cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(x, y, z));
```

- **x, y, z**: The distances to move the cube along the respective axes.

### Scaling

To resize the cube uniformly or along specific axes:

```cpp
// Uniform scaling
cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(scaleFactor));

// Non-uniform scaling
cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(scaleX, scaleY, scaleZ));
```

- **scaleFactor**: A single value to scale uniformly.
- **scaleX, scaleY, scaleZ**: Scale factors along the X, Y, and Z axes.

### Rotation

To rotate the cube around an axis:

```cpp
cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(angle), glm::vec3(axisX, axisY, axisZ));
```

- **angle**: The rotation angle in degrees.
- **axisX, axisY, axisZ**: The axis of rotation.

## Drawing the Cube

After setting up transformations, compute the necessary matrices and update the shader uniforms:

```cpp
glm::mat4 cubeMVMatrix = ViewMatrix * cubeModelMatrix;
glm::mat4 cubeMVPMatrix = ProjMatrix * cubeMVMatrix;
glm::mat4 cubeNormalMatrix = glm::transpose(glm::inverse(cubeMVMatrix));

// Send matrices to the shader
glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeMVMatrix));
glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeMVPMatrix));
glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeNormalMatrix));
```

Finally, draw the cube:

```cpp
glBindVertexArray(cubeVAO);
glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
```

## Examples

### Example 1: Positioning a Cube Above the origin Sphere

To place a cube 2 units above the origin:

```cpp
glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0.0f, 2.0f, 0.0f));
```

### Example 2: Scaling a Cube on Different Axes

To scale the cube to double its size along the X-axis and half along the Y-axis:

```cpp
cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(2.0f, 0.5f, 1.0f));
```

### Example 3: Rotating a Cube Around the Y-Axis

To rotate the cube 45 degrees around the Y-axis:

```cpp
cubeModelMatrix = glm::rotate(cubeModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
```

## Additional Notes

- **Combining Transformations**: Scale first, then rotate, then translate.
- **Textures**: The cube's vertices include texture coordinates, allowing mapping textures onto its faces.
- **Lighting**: Normals are defined for each face, enabling lighting calculations in shaders.

---

current code supports :

- **UV Mapping**: The cube's vertices include UV coordinates, which are necessary for texture mapping.
- **Texture Application**: we can load texture images and apply them to the cube by binding a texture object before drawing.
- **Shader Support**: Ensure our fragment shader samples from a texture sampler and applies it to the fragment color.
- **Multiple Textures**: If we have multiple cubes and want different textures on each, we can bind the appropriate texture before drawing each cube.

**Steps to Apply Textures:**

1. **Load Texture Image**: Use an image loading library to load the texture image into memory.
2. **Generate Texture Object**: Create a texture object with `glGenTextures`.
3. **Bind and Set Texture Parameters**: Bind the texture and set parameters like wrapping and filtering.
4. **Upload Texture Data**: Use `glTexImage2D` to upload the texture data to the GPU.
5. **Modify Shaders**: Ensure shaders are set up to use texture samplers and sample the texture using the UV coordinates.
6. **Bind Texture Before Drawing**: Bind the texture before drawing the cube.
