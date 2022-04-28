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
        {glm::vec4(2.0f,0,0,0), glm::vec2(0,0), 2, PrimitiveType::Sphere ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))},
        {glm::vec4(1.0f,2,3,0), glm::vec2(0,0), 2, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))},    
        // {glm::vec4(1.0f,2,3,0), glm::vec2(0,0), 2, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 5, 5)))},    
        // {glm::vec4(2.0f,1,1,1), glm::vec2(0,0), 1, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, -5, 0)))}        
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
    renderGroups[0].numPrimitives = primitives.size();
    renderGroups[0].opType = RenderOpType::Classic;
    
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
            {glm::vec4(1.0f,1,1,0), glm::vec2(0,0), 1, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Sphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f,0,0,0), glm::vec2(0,0), 1, PrimitiveType::Sphere ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::RoundBox)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,1.0f,0.5), glm::vec2(0,0), 1, PrimitiveType::RoundBox ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::BoxFrame)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,1.0f,0.2), glm::vec2(0,0), 1, PrimitiveType::BoxFrame ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Torus)
    {
        primitives.push_back(
            {glm::vec4(1.0f,1.0f,0,0), glm::vec2(0,0), 1, PrimitiveType::Torus ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::CappedTorrus)
    {
        primitives.push_back(
            {glm::vec4(sin(1),cos(1),0.4,0.1), glm::vec2(0,0), 1, PrimitiveType::CappedTorrus ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Link)
    {
        primitives.push_back(
            {glm::vec4(1.0f, 0.6f, 0.3f, 0.0f), glm::vec2(1.0f), 1, PrimitiveType::Link ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::InfiniteCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::InfiniteCylinder ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Cone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Cone ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::InfiniteCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::InfiniteCone ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Plane)
    {
        primitives.push_back(
            {glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f), 1, PrimitiveType::Plane ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::HexaPrism)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::HexaPrism ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::TriPrism)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::TriPrism ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Capsule)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Capsule ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::CappedCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::CappedCylinder ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::RoundCylinder)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::RoundCylinder ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::CappedCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f, 1.0f, 0.4f, 0.0f), glm::vec2(1.0f), 1, PrimitiveType::CappedCone ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::SolidAngle)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::SolidAngle ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::CutSphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::CutSphere ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::CutHollowSphere)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::CutHollowSphere ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::DeathStar)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::DeathStar ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::RoundCone)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::RoundCone ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Ellipsoid)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Ellipsoid ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Rhombus)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Rhombus ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Octahedron)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Octahedron ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Pyramid)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Pyramid ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Triangle)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Triangle ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    else if(primType == PrimitiveType::Quad)
    {
        primitives.push_back(
            {glm::vec4(1.0f), glm::vec2(1.0f), 1, PrimitiveType::Quad ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
        );
    }
    renderGroups[group].primitives[renderGroups[group].numPrimitives++] = (int)primitives.size()-1;

    //Update render group buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, group * sizeof(RenderGroup), sizeof(RenderGroup), &renderGroups[group]); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);     

    //Update primitives buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, primitives.size() * sizeof(Primitive), primitives.data(),  GL_STATIC_READ); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);  
}

void Template::AddGroup()
{
    renderGroups.push_back({});
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
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, primitiveIndex * sizeof(Primitive) + offsetof(Primitive, inverseTransform), sizeof(glm::mat4), &invMat); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);      
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
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, groupIndex * sizeof(RenderGroup) + offsetof(RenderGroup, inverseTransform), sizeof(glm::mat4), &invMat); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);      
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
        }
        ImGui::EndPopup();
    }                        
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

                std::string groupLabel = "Group " + std::to_string(i);
                bool groupOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, groupLabel.c_str());

                AddPrimContextMenu(groupLabel, i);
                                
                if (ImGui::IsItemClicked())
                {
                    clickedGroup = i;
                    selectedPrimitiveInGroup=-1;
                }
                
                //If the group is open, render its children
                if (groupOpen)
                {
                    ImGui::TreePush();
                    for(int j=0; j<renderGroups[i].numPrimitives; j++)
                    {
                        const bool isPrimitiveSelected = (selectedPrimitiveInGroup == j && selectedGroup == i);
                        int primInx = renderGroups[i].primitives[j];

                        std::string label = std::string(PrimitiveNames[(int)primitives[primInx].Type]) + " " + std::to_string(j);
                        if (ImGui::Selectable(label.c_str(), isPrimitiveSelected))
                        {
                            selectedPrimitiveInGroup = j;
                            selectedGroup=i;
                        }
                        if (isPrimitiveSelected)
                            ImGui::SetItemDefaultFocus();                    
                    }

                    ImGui::TreePop();
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
    if(selectedGroup >=0)
    {
        if(ImGui::CollapsingHeader("Group Properties"))
        {
            int opType = (int)renderGroups[selectedGroup].opType;
            bool updateRenderGroupData=false;
            if (ImGui::Combo("Medium Type", &opType, "Classic\0Smooth\0Subtract\0Smooth Subtract\0Intersect\0Smooth Intersect\0"))
            {
                updateRenderGroupData = true;
                renderGroups[selectedGroup].opType = (RenderOpType)opType;
            }  
            if(updateRenderGroupData)
            {
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, selectedGroup * sizeof(RenderGroup), sizeof(RenderGroup), &renderGroups[selectedGroup]); 
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            }                      
        }
    }

    //Properties
    if(selectedGroup >=0 && selectedPrimitiveInGroup>=0 ){        
        int selectedPrimitive = renderGroups[selectedGroup].primitives[selectedPrimitiveInGroup];
        Primitive *primitive = &primitives[selectedPrimitive];
        if(ImGui::CollapsingHeader("Primitive Properties"))
        {
            bool updatePrimitiveData=false;
            if(primitive->Type == PrimitiveType::Sphere)
            {
                updatePrimitiveData |= ImGui::SliderFloat("Radius", &primitive->Data1.x, 0, 10);
            }
            else if(primitive->Type == PrimitiveType::Box)
            {
                updatePrimitiveData |= ImGui::SliderFloat3("Size", &primitive->Data1.x, 0, 10);
            }
            else if(primitive->Type == PrimitiveType::RoundBox)
            {
                updatePrimitiveData |= ImGui::SliderFloat3("Size", &primitive->Data1.x, 0, 10);
                updatePrimitiveData |= ImGui::SliderFloat("Round Factor", &primitive->Data1.w, 0, 1);
            }

            if(updatePrimitiveData)
            {
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, selectedPrimitive * sizeof(Primitive) + offsetof(Primitive, Data1), sizeof(glm::vec4) + sizeof(glm::vec2), &primitive->Data1[0]); 
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            }
        }

        ImGui::Separator();

        //TODO(Jacques): Have it static

        int matID = (int)primitive->matID;
        const char* current_item = matNames[matID].c_str();
        bool updateMaterial=false;
        if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < materials.size(); n++)
            {
                bool is_selected = (current_item == matNames[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(matNames[n].c_str(), is_selected))
                {
                    current_item = matNames[n].c_str();
                    primitive->matID = n;
                    updateMaterial=true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
        }

        if(updateMaterial)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, selectedPrimitive * sizeof(Primitive) + offsetof(Primitive, matID), sizeof(int), &primitive->matID); 
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);            
        }

        RenderMaterial(primitive->matID);        
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
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(Material), sizeof(Material), material); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);        
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
    // ImGui::ShowDemoWindow();

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
