#pragma once
#include "../Demo.hpp"
#include "GL_Helpers/GL_Shader.hpp"
#include "GL_Helpers/GL_Camera.hpp"

#include "GL_Helpers/GL_Mesh.hpp"
#include "GL_Helpers/GL_Camera.hpp"


#include "imgui.h"
#include "ImGuizmo.h"
//Render groups
//

enum RenderOpType
{
    Classic = 0,
    Smooth = 1,
    Subtract = 2,
    SmoothSubtract = 3,
    Intersect = 4,
    SmoothIntersect = 5,
};


//Data :
//Sphere: (Radius, X, X, X), (X, X)
//Box: (W, H, D, X), (X, X)
//RoundBox: (W, H, D, RF), (X, X)

enum PrimitiveType
{
    Sphere=0,
    Box=1,
    RoundBox=2,
    BoxFrame=3,
    Torus=4,
    CappedTorrus=5,
    Link=6,
    InfiniteCylinder=7,
    Cone=8,
    InfiniteCone=9,
    Plane=10,
    HexaPrism=11,
    TriPrism=12,
    Capsule=13,
    CappedCylinder=14,
    RoundCylinder=15,
    CappedCone=16,
    SolidAngle=17,
    CutSphere=18,
    CutHollowSphere=19,
    DeathStar=20,
    RoundCone=21,
    Ellipsoid=22,
    Rhombus=23,
    Octahedron=24,
    Pyramid=25,
    Triangle=26,
    Quad=27,
    Count
};


struct Primitive
{
    glm::vec4 Data1;
    glm::vec4 Data2;

    glm::vec3 Ellongation;
    float rounding;
    
    int matID;
    PrimitiveType Type;
    RenderOpType opType;
    int symmetry;

    glm::vec4 repetitionData;
    glm::vec4 repetitionBounds;
    
    glm::mat4 inverseTransform;
};


struct RenderGroup
{
    int padding0;
    int numPrimitives;
    glm::vec2 padding;
    glm::mat4 inverseTransform;
    int primitives[256];
};

struct Material
{
    glm::vec3 Color;
    float shininess;
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

    void AddPrimitiveToGroup(PrimitiveType primType, int group);
    void AddGroup();
    void AddPrimContextMenu(std::string groupLabel, int groupIndex);
    void DeletePrimitiveFromGroup(int groupInx, int primInx);
    void DuplicatePrimitive(int groupInx, int primInx);
    
    //GPU upload
    void UploadGroupData(int groupInx);
    void UploadAllGroupData();
    void UploadPrimData(int primInx);
    void UploadPrimDataFrom(int primInx);
    void UploadMatData(int matInx);
    void UploadAllPrimData();
    
    

    //ImGui
    void HandleTransforms();
    void HandlePrimitiveTransform(int primitiveIndex, int groupIndex);
    void HandleGroupTransform(int groupIndex);
    
    void RenderSceneTree();
    void RenderProperties();

    void RenderMaterialsTab();
    void RenderMaterial(int materialInx);

    void RenderTransformParams();

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
    std::vector<std::string> primNames;   
    
    std::vector<Material> materials;
    std::vector<std::string> matNames;

    std::vector<RenderGroup> renderGroups;
    std::vector<std::string> groupNames;

    GLuint primitivesBuffer;
    GLuint renderGroupsBuffer;
    GLuint materialsBuffer;



    //Scene tree
    int selectedGroup = -1;
    int selectedPrimitiveInGroup = -1;    

    ImGuizmo::OPERATION currentGizmoOperation = (ImGuizmo::TRANSLATE);
    ImGuizmo::MODE currentGizmoMode = (ImGuizmo::WORLD); 

    
};