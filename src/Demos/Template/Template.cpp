#include "Template.hpp"

#include "GL/glew.h"
#include <glm/gtx/quaternion.hpp>

#include "GL_Helpers/Util.hpp"
#include <fstream>
#include <sstream>
#include <random>



const char *PrimitiveNames[] =
{
    "Sphere",
    "Box",
    "RoundBox",
    "BoxFrame",
    "Torus",
    "CappedTorrus",
    "Link",
    "InfiniteCylinder",
    "Cone",
    "InfiniteCone",
    "Plane",
    "HexaPrism",
    "TriPrism",
    "Capsule",
    "CappedCylinder",
    "RoundCylinder",
    "CappedCone",
    "SolidAngle",
    "CutSphere",
    "CutHollowSphere",
    "DeathStar",
    "RoundCone",
    "Ellipsoid",
    "Rhombus",
    "Octahedron",
    "Pyramid",
    "Triangle",
    "Quad",
    "Count"
};


Template::Template() {
}

void Template::Load() {

    MeshShader = GL_Shader("shaders/MeshShader.vert", "", "shaders/MeshShader.frag");

    Quad = GetQuad();

    lightDirection = glm::normalize(glm::vec3(0, -1, 1));

    cam = GL_Camera(glm::vec3(0, 0, 0));  

    primitives = 
    {
        {glm::vec4(2.0f,0,0,0), glm::vec4(0), glm::vec3(0), 0, 2, PrimitiveType::Sphere ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1),glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))},
        {glm::vec4(1.0f,2,3,0), glm::vec4(0), glm::vec3(0), 0, 2, PrimitiveType::Box ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1),glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))},    
        // {glm::vec4(1.0f,2,3,0), glm::vec2(0,0), 2, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 5, 5)))},    
        // {glm::vec4(2.0f,1,1,1), glm::vec2(0,0), 1, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, -5, 0)))}        
    };
    primNames = 
    {
        "Sphere",
        "Box"
    };

    glGenBuffers(1, &primitivesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, primitives.size() * sizeof(Primitive), primitives.data(),  GL_STATIC_READ); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, primitivesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    materials = 
    {
        {glm::vec3(1, 0, 0), 0},
        {glm::vec3(0, 1, 0), 0},
        {glm::vec3(0, 0, 1), 0},
    };
	glGenBuffers(1, &materialsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(Material), materials.data(),  GL_STATIC_READ); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, materialsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    for(int i=0; i<materials.size(); i++)
    {
        matNames.push_back(("Material" + std::to_string(i)));
    }


    renderGroups.resize(1);
    for(int i=0; i<primitives.size(); i++)
    {
        renderGroups[0].primitives[i] = i;
    }
    renderGroups[0].numPrimitives = (int)primitives.size();
    groupNames = {"Group 0"};
    
	glGenBuffers(1, &renderGroupsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, renderGroups.size() * sizeof(RenderGroup), renderGroups.data(),  GL_STATIC_READ); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, renderGroupsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Template::AddPrimitiveToGroup(PrimitiveType primType, int group)
{
    if(primType==PrimitiveType::Box)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1,1,0), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::Box ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Box" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Sphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f,0,0,0), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::Sphere ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Sphere" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::RoundBox)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,1.0f,0.5), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::RoundBox ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("RoundBox" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::BoxFrame)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,1.0f,0.2), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::BoxFrame ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("BoxFrame" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Torus)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,0,0), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::Torus ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Torus" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::CappedTorrus)
    {
        primitives.push_back(
            {glm::vec4(sin(1),cos(1),0.4,0.1), glm::vec4(0), glm::vec3(0), 0,1, PrimitiveType::CappedTorrus ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("CappedTorrus" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Link)
    {
        primitives.push_back(
            {glm::vec4(1.0f, 0.6f, 0.3f, 0.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Link ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Link" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::InfiniteCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::InfiniteCylinder ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("InfiniteCylinder" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Cone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Cone ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Cone" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::InfiniteCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::InfiniteCone ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("InfiniteCone" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Plane)
    {
        primitives.push_back(
            {glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Plane ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Plane" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::HexaPrism)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::HexaPrism ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("HexaPrism" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::TriPrism)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::TriPrism ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("TriPrism" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Capsule)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Capsule ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Capsule" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::CappedCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::CappedCylinder ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("CappedCylinder" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::RoundCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::RoundCylinder ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("RoundCylinder" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::CappedCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f, 1.0f, 0.4f, 0.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::CappedCone ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("CappedCone" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::SolidAngle)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::SolidAngle ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("SolidAngle" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::CutSphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::CutSphere ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("CutSphere" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::CutHollowSphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::CutHollowSphere ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("CutHollowSphere" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::DeathStar)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::DeathStar ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("DeathStar" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::RoundCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::RoundCone ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("RoundCone" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Ellipsoid)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Ellipsoid ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Ellipsoid" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Rhombus)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Rhombus ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Rhombus" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Octahedron)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Octahedron ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Octahedron" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Pyramid)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Pyramid ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Pyramid" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Triangle)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Triangle ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Triangle" + std::to_string(primitives.size()));
    }
    else if(primType == PrimitiveType::Quad)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec4(1.0f), glm::vec3(0), 0,1, PrimitiveType::Quad ,RenderOpType::Classic,0,glm::vec4(10,10,10,0),glm::vec4(1), glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
        primNames.push_back("Quad" + std::to_string(primitives.size()));
    }
    renderGroups[group].primitives[renderGroups[group].numPrimitives++] = (int)primitives.size()-1;

    //Update render group buffer
    UploadGroupData(group);
    
    //Update primitives buffer
    UploadAllPrimData();
}

void Template::AddGroup()
{
    renderGroups.push_back({});
    groupNames.push_back("Group" + std::to_string(groupNames.size()));
    UploadAllGroupData();
}

void Template::UploadAllGroupData()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, renderGroups.size() * sizeof(RenderGroup), renderGroups.data(),  GL_STATIC_READ); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);     
}

void Template::HandlePrimitiveTransform(int primitiveIndex, int groupIndex)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    glm::mat4 modelMatrix = glm::inverse(primitives[primitiveIndex].inverseTransform);
    glm::mat4 groupModelMatrixInverse = renderGroups[groupIndex].inverseTransform;
    glm::mat4 groupModelMatrix = glm::inverse(groupModelMatrixInverse);
    glm::mat4 worldModelMatrix = groupModelMatrix * modelMatrix;

    bool changed = ImGuizmo::Manipulate(&cam.GetViewMatrix()[0][0], &cam.GetProjectionMatrix()[0][0], currentGizmoOperation, currentGizmoMode, &worldModelMatrix[0][0], NULL, NULL);    
    if(changed)
    {
        //Remove group transform to find back only the model matrix, and invert it
        modelMatrix = groupModelMatrixInverse * worldModelMatrix;
        glm::mat4 invMat = glm::inverse(modelMatrix);

        primitives[primitiveIndex].inverseTransform = invMat;
        
        UploadPrimData(primitiveIndex);
    }
}

void Template::HandleGroupTransform(int groupIndex)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    glm::mat4 modelMatrix = glm::inverse(renderGroups[groupIndex].inverseTransform);
    bool changed = ImGuizmo::Manipulate(&cam.GetViewMatrix()[0][0], &cam.GetProjectionMatrix()[0][0], currentGizmoOperation, currentGizmoMode, &modelMatrix[0][0], NULL, NULL);    
    if(changed)
    {
        glm::mat4 invMat = glm::inverse(modelMatrix);
        renderGroups[groupIndex].inverseTransform = invMat;
        UploadGroupData(groupIndex);
    }
}

void Template::AddPrimContextMenu(std::string groupLabel, int groupIndex)
{
    if (ImGui::BeginPopupContextItem(groupLabel.c_str(), 1))
    {
        bool shouldAdd=false;
        PrimitiveType addType = PrimitiveType::Box;

        for(int i=0; i<PrimitiveType::Count; i++)
        {
            if(ImGui::Button(PrimitiveNames[i]))
            {
                shouldAdd=true;
                addType = (PrimitiveType)i;
                ImGui::CloseCurrentPopup();
            }
        }

        if(shouldAdd)
        {
            AddPrimitiveToGroup(addType, groupIndex);      
            selectedPrimitiveInGroup = renderGroups[groupIndex].numPrimitives-1;                          
        }
        ImGui::EndPopup();
    }                        
}

void Template::UploadGroupData(int groupInx)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, groupInx * sizeof(RenderGroup), sizeof(RenderGroup), &renderGroups[groupInx]); 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    
}

void Template::UploadMatData(int MatInx)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, MatInx * sizeof(Material), sizeof(Material), &materials[MatInx]); 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    
}

void Template::UploadPrimData(int primInx)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, primInx * sizeof(Primitive), sizeof(Primitive), &primitives[primInx]); 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    
}
void Template::UploadPrimDataFrom(int primInx)
{
    int numToUpload = (int)primitives.size() - primInx;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, primInx * sizeof(Primitive), numToUpload * sizeof(Primitive), &primitives[primInx]); 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    
}
void Template::UploadAllPrimData()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, primitives.size() * sizeof(Primitive), primitives.data(),  GL_STATIC_READ); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);      
}

void Template::DeletePrimitiveFromGroup(int groupInx, int primInx)
{
    int globalPrimInx = renderGroups[groupInx].primitives[primInx];

    //Delete from array
    for(int i=primInx; i<renderGroups[groupInx].numPrimitives-1; i++)
    {
        renderGroups[groupInx].primitives[i] = renderGroups[groupInx].primitives[i+1]; 
    }
    renderGroups[groupInx].numPrimitives--;

    //Remap all the other primitives
	for (int i = 0; i < renderGroups.size(); i++)
	{
	    for (int j = 0; j < renderGroups[i].numPrimitives; j++)
        {
            if (renderGroups[i].primitives[j] >= globalPrimInx)
            {
                renderGroups[i].primitives[j]--;
            }
        }
	}

    //Delete from prim vector
    primitives.erase(primitives.begin() + globalPrimInx);

    //Delete from names
    primNames.erase(primNames.begin() + globalPrimInx);



    //Re upload from gpu group onwards
	UploadGroupData(groupInx);
    //Re upload from gpu prims onwards
    UploadPrimDataFrom(0);
}

void Template::DuplicatePrimitive(int groupInx, int primInx)
{
    int globalPrimInx = renderGroups[groupInx].primitives[primInx];
    Primitive prim = primitives[globalPrimInx];
    
    //Duplicate primitive
    primitives.push_back(prim);
    primNames.push_back(primNames[globalPrimInx] + "Dup");

    //Add it to the group
    renderGroups[groupInx].primitives[renderGroups[groupInx].numPrimitives++] = (int)primitives.size()-1;

    UploadGroupData(groupInx);
    UploadAllPrimData();
}

void Template::RenderSceneTree()
{
    if (ImGui::TreeNode("Primitives"))
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

        //Global context menu
        if (ImGui::BeginPopupContextItem("item context menu", 1))
        {
            if(ImGui::Button("Group"))
            {
                AddGroup();
                selectedGroup = (int)renderGroups.size()-1;    
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        
        int clickedGroup = -1;
        for (int i = 0; i < renderGroups.size(); i++)
        {
            ImGuiTreeNodeFlags node_flags = base_flags;
            const bool isGroupSelected = (selectedGroup == i);
            if (isGroupSelected)
                node_flags |= ImGuiTreeNodeFlags_Selected;

                std::string groupName = groupNames[i];
                bool groupOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, groupName.c_str());

                AddPrimContextMenu(groupName, i);
                                
                if (ImGui::IsItemClicked())
                {
                    clickedGroup = i;
                    selectedPrimitiveInGroup=-1;
                }
                
                //If the group is open, render its children
                if (groupOpen)
                {
                    for(int j=0; j<renderGroups[i].numPrimitives; j++)
                    {
                        const bool isPrimitiveSelected = (selectedPrimitiveInGroup == j && selectedGroup == i);
                        int primInx = renderGroups[i].primitives[j];

                        ImGui::PushID(j);
                        if (ImGui::Selectable(primNames[primInx].c_str(), isPrimitiveSelected))
                        {
                            selectedPrimitiveInGroup = j;
                            selectedGroup=i;                            
                        }

                        //Handle drag / drop
                        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                        {
                            int n_next = j + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                            if (n_next >= 0 && n_next < renderGroups[i].numPrimitives)
                            {
                                renderGroups[i].primitives[j] = renderGroups[i].primitives[n_next];
                                renderGroups[i].primitives[n_next] = primInx;
                                UploadGroupData(i);
                                ImGui::ResetMouseDragDelta();
                            }
                        }                      

                        if (ImGui::BeginPopupContextItem("Prim Menu", 1))
                        {
                            if(ImGui::Button("Delete"))
                            {
                                DeletePrimitiveFromGroup(i, j);
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }                      
                        if (isPrimitiveSelected)
                            ImGui::SetItemDefaultFocus();                    
                        
                        ImGui::PopID();
                    }


                    ImGui::TreePop();
                }
        }
        if (clickedGroup != -1)
        {
            selectedGroup = clickedGroup;           // Click to single-select
        }
        ImGui::TreePop();
    }

    ImGui::Text(std::to_string(selectedGroup).c_str());
    ImGui::Text(std::to_string(selectedPrimitiveInGroup).c_str());
}

bool VectorOfStringGetter(void* data, int n, const char** out_text)
{
    const std::vector<std::string>* v = (std::vector<std::string>*)data;
    *out_text = v->at(n).c_str();
    return true;
}

void Template::RenderProperties()
{
    //Properties
    if(selectedGroup >=0 && selectedPrimitiveInGroup>=0 ){        
        int selectedPrimitive = renderGroups[selectedGroup].primitives[selectedPrimitiveInGroup];
        Primitive *primitive = &primitives[selectedPrimitive];
        if(ImGui::CollapsingHeader("Primitive Properties"))
        {
            ImGui::TreePush();
            bool updatePrimitiveData=false;
            if(ImGui::CollapsingHeader("Geometry"))
            {
                if(primitive->Type == PrimitiveType::Sphere)
                {
                    updatePrimitiveData |= ImGui::DragFloat("Radius", &primitive->Data1.x, 0.2f);
                }
                else if(primitive->Type == PrimitiveType::Box)
                {
                    updatePrimitiveData |= ImGui::DragFloat3("Size", &primitive->Data1.x, 0.2f);
                }
                else if(primitive->Type == PrimitiveType::RoundBox)
                {
                    updatePrimitiveData |= ImGui::DragFloat3("Size", &primitive->Data1.x, 0.2f);
                    updatePrimitiveData |= ImGui::DragFloat("Round Factor", &primitive->Data1.w, 0.02f);
                }
            }

            ImGui::Separator();
            {
                {
                    int opType = (int)primitive->opType;
                    if (ImGui::Combo("Operation", &opType, "Classic\0Smooth\0Subtract\0Smooth Subtract\0Intersect\0Smooth Intersect\0"))
                    {
                        updatePrimitiveData = true;
                        primitive->opType = (RenderOpType)opType;
                    }                        
                }
            }
            ImGui::Separator();

            if(ImGui::CollapsingHeader("Modifiers"))
            {
                updatePrimitiveData |= ImGui::DragFloat3("Ellongation", &primitive->Ellongation[0], 0.2f);
                updatePrimitiveData |= ImGui::DragFloat("Rounding", &primitive->rounding, 0.02f);
                
                int symmetry = (int)primitive->symmetry;
                if (ImGui::Combo("Symmetry", &symmetry, "None\0X\0Z\0Y\0XZ\0XY\0YZ\0XYZ\0"))
                {
                    updatePrimitiveData = true;
                    primitive->symmetry = symmetry;
                }
                ImGui::Separator();
                int repetition = (int)primitive->repetitionData.w;
                if (ImGui::Combo("repetition", &repetition, "None\0Finite\0Infinite\0"))
                {
                    updatePrimitiveData = true;
                    primitive->repetitionData.w = (float)repetition;
                }
                ImGui::Separator();
                if(repetition > 0)
                {
                    if(repetition ==1)
                    {
                        updatePrimitiveData |= ImGui::DragFloat3("Bounds", &primitive->repetitionBounds[0], 1);
                        updatePrimitiveData |= ImGui::DragFloat("Spacing", &primitive->repetitionBounds[3], 0.2f);
                    }
                    else
                    {
                        updatePrimitiveData |= ImGui::DragFloat3("Grid", &primitive->repetitionData[0], 0.2f);
                    }
                }
            }

            if(updatePrimitiveData)
            {
                UploadPrimData(selectedPrimitive);
            }


            ImGui::Separator();

            if(ImGui::CollapsingHeader("Material"))
            {
                int matID = (int)primitive->matID;
                const char* current_item = matNames[matID].c_str();
                bool updateMaterialID=false;
                if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
                {
                    for (int n = 0; n < materials.size(); n++)
                    {
                        bool is_selected = (current_item == matNames[n]); // You can store your selection however you want, outside or inside your objects
                        if (ImGui::Selectable(matNames[n].c_str(), is_selected))
                        {
                            current_item = matNames[n].c_str();
                            primitive->matID = n;
                            updateMaterialID=true;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                    ImGui::EndCombo();
                }

                if(updateMaterialID)
                {
                    UploadPrimData(selectedPrimitive);
                }

                RenderMaterial(primitive->matID);        
            }

            ImGui::TreePop();
        }
    }
}

void Template::HandleTransforms()
{
    bool hasTransform=false;
    if(selectedGroup >=0 && selectedPrimitiveInGroup>=0 ){        
        //Transform
        int selectedPrimitive = renderGroups[selectedGroup].primitives[selectedPrimitiveInGroup];
        HandlePrimitiveTransform(selectedPrimitive, selectedGroup);
        hasTransform=true;
    }    

    if(selectedGroup >=0 && selectedPrimitiveInGroup<0)
    {
        HandleGroupTransform(selectedGroup);
        hasTransform=true;
    }    

    if(hasTransform) 
    {
        if (ImGui::IsKeyPressed(0x54))
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(0x52))
            currentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(0x53)) // r Key
            currentGizmoOperation = ImGuizmo::SCALE;
    }
}

void Template::RenderMaterial(int index)
{
    std::string label = matNames[index];
    Material *material = &materials[index];
    bool updateMaterial=false;
    if(ImGui::CollapsingHeader(label.c_str()))
    {
        ImGui::PushID(index);
        updateMaterial |= ImGui::ColorEdit3("Color", &material->Color[0]);
        ImGui::PopID();
    }

    if(updateMaterial)
    {
        UploadMatData(index);      
    }
}

void Template::RenderMaterialsTab()
{
    if(ImGui::Button("Add"))
    {
        materials.push_back({glm::vec3(0.8, 0.8, 0.8), 0});
        matNames.push_back(("Material" + std::to_string(materials.size()-1)));

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(Material), materials.data(),  GL_STATIC_READ); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);        
    }
    for(int i=0; i<materials.size(); i++)
    {
        RenderMaterial(i);
    }
}

void Template::RenderTransformParams()
{
    if(ImGui::CollapsingHeader("Transform Params"))
    {
        if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
            currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
            currentGizmoOperation = ImGuizmo::SCALE;

        if (currentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
                currentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
                currentGizmoMode = ImGuizmo::WORLD;
        }         
    }
}

void Template::RenderGUI() {
        
    
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(250, (float)windowHeight), ImGuiCond_Appearing);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
    if (!ImGui::Begin("Dear ImGui Demo", NULL, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            RenderSceneTree();
            RenderProperties();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Materials"))
        {
            RenderMaterialsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Parameters"))
        {
            RenderTransformParams();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGuiIO& io = ImGui::GetIO();
    if(selectedGroup >=0 && selectedPrimitiveInGroup >=0)
    {
        if(io.KeyCtrl && ImGui::IsKeyPressed(68))
        {
            DuplicatePrimitive(selectedGroup, selectedPrimitiveInGroup);
            selectedPrimitiveInGroup = renderGroups[selectedGroup].numPrimitives-1;
        }
    }

    HandleTransforms();
    

    
    if(ImGui::IsAnyItemActive()|| ImGuizmo::IsUsing()) cam.locked=true;
    else cam.locked=false;
    ImGui::End();
}

void Template::Render() {
    glUseProgram(MeshShader.programShaderObject);
    glUniform3fv(glGetUniformLocation(MeshShader.programShaderObject, "camPosition"), 1, glm::value_ptr(cam.worldPosition));
    glUniformMatrix4fv(glGetUniformLocation(MeshShader.programShaderObject, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(cam.GetModelMatrix()));
    
    glUniform1i(glGetUniformLocation(MeshShader.programShaderObject, "numPrimitives"), (GLint)primitives.size());
    glUniform1i(glGetUniformLocation(MeshShader.programShaderObject, "numGroups"), (GLint)renderGroups.size());

    glUniform1f(glGetUniformLocation(MeshShader.programShaderObject, "camFov"), 0.0174533f * cam.GetFov());
    glUniform2f(glGetUniformLocation(MeshShader.programShaderObject, "resolution"), (GLfloat)windowWidth, (GLfloat)windowHeight);
    

    Quad->RenderShader(MeshShader.programShaderObject);
}

void Template::Unload() {
    Quad->Unload();
    delete Quad;
}


void Template::MouseMove(float x, float y) {
    cam.mouseMoveEvent(x, y);
}

void Template::LeftClickDown() {
    cam.mousePressEvent(0);
}

void Template::LeftClickUp() {
    cam.mouseReleaseEvent(0);
}

void Template::RightClickDown() {
    cam.mousePressEvent(1);
}

void Template::RightClickUp() {
    cam.mouseReleaseEvent(1);
}

void Template::Scroll(float offset) {
    cam.Scroll(offset);
}
