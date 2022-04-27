#pragma once
#include "../Demo.hpp"
#include "GL_Helpers/GL_Shader.hpp"
#include "GL_Helpers/GL_Camera.hpp"

#include "GL_Helpers/GL_Mesh.hpp"
#include "GL_Helpers/GL_Camera.hpp"

//Render groups
//

enum RenderOpType
{
    Classic = 0,
    Smooth = 0,
};

struct RenderGroup
{
    RenderOpType opType;
    int numPrimitives;
    glm::vec2 padding;
    glm::mat4 transform;
    int primitives[256];
};

struct Material
{
    glm::vec3 Color;
    float padding;
};

//Data :
//Sphere: (Radius, X, X, X), (X, X)
//Box: (W, H, D, X), (X, X)

enum PrimitiveType
{
    Sphere=0,
    Box=1
};

struct Primitive
{
    glm::vec4 Data1;
    glm::vec2 Data2;
    int matID;
    int Type;
    glm::mat4 transform;
};

class Template : public Demo {
public : 
    Template();
    void Load();
    void Render();
    void RenderGUI();
    void Unload();

    void MouseMove(float x, float y);
    void LeftClickDown();
    void LeftClickUp();
    void RightClickDown();
    void RightClickUp();
    void Scroll(float offset);

private:
    clock_t t;
    float deltaTime;
    float elapsedTime;
        
    GL_Camera cam;

    GL_Shader MeshShader;
    GL_Mesh * Quad;

    glm::vec3 lightDirection;

    bool lightDirectionChanged=false;

    bool specularTextureSet=false;
    bool normalTextureSet=false;

    std::vector<Primitive> primitives;
    std::vector<Material> materials;

    std::vector<RenderGroup> renderGroups;

    GLuint primitivesBuffer;
    GLuint renderGroupsBuffer;
    GLuint materialsBuffer;
};