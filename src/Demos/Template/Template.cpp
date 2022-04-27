#include "Template.hpp"

#include "GL/glew.h"
#include <glm/gtx/quaternion.hpp>

#include "GL_Helpers/Util.hpp"
#include <fstream>
#include <sstream>
#include <random>

#include "imgui.h"
#include "ImGuizmo.h"
Template::Template() {
}

void Template::Load() {

    MeshShader = GL_Shader("shaders/MeshShader.vert", "", "shaders/MeshShader.frag");

    Quad = GetQuad();

    lightDirection = glm::normalize(glm::vec3(0, -1, 1));

    cam = GL_Camera(glm::vec3(0, 0, 0));  

    primitives = 
    {
        {glm::vec4(2.5f,0,0,0), glm::vec2(0,0), 1, PrimitiveType::Sphere ,glm::inverse(glm::mat4(1.0))},
        {glm::vec4(2.0f,0,0,0), glm::vec2(0,0), 2, PrimitiveType::Sphere ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(-5, 0, 0)))},
        {glm::vec4(1.0f,2,3,0), glm::vec2(0,0), 2, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 5, 0)))},    
        {glm::vec4(2.0f,1,1,0), glm::vec2(0,0), 1, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, -5, 0)))}        
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

    renderGroups.resize(1);
    renderGroups[0].primitives[0] = 0;
    renderGroups[0].primitives[1] = 1;
    renderGroups[0].primitives[2] = 2;
    renderGroups[0].primitives[3] = 3;
    renderGroups[0].numPrimitives = 4;
    renderGroups[0].opType = RenderOpType::Classic;
    
	glGenBuffers(1, &renderGroupsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, renderGroups.size() * sizeof(RenderGroup), renderGroups.data(),  GL_STATIC_READ); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, renderGroupsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

    static int selectedGroup = -1;
    static int selectedPrimitiveInGroup = -1;
    if (ImGui::TreeNode("Primitives"))
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

        if (ImGui::BeginPopupContextItem("item context menu", 1))
        {
            if(ImGui::Button("Group"))
            {
                renderGroups.push_back({});
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
                glBufferData(GL_SHADER_STORAGE_BUFFER, renderGroups.size() * sizeof(RenderGroup), renderGroups.data(),  GL_STATIC_READ); 
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);                
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        
        int clickedGroup = -1;
        int clickedPrim = -1;
        for (int i = 0; i < renderGroups.size(); i++)
        {
            ImGuiTreeNodeFlags node_flags = base_flags;
            const bool isGroupSelected = (selectedGroup == i);
            if (isGroupSelected)
                node_flags |= ImGuiTreeNodeFlags_Selected;

                std::string groupLabel = "Group " + std::to_string(i);
                bool groupOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, groupLabel.c_str());

                if (ImGui::BeginPopupContextItem(groupLabel.c_str(), 1))
                {
                    if(ImGui::Button("Cube"))
                    {
                        primitives.push_back(
                            {glm::vec4(1.0f,1,1,0), glm::vec2(0,0), 1, PrimitiveType::Box ,glm::inverse(glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)))}        
                        );
                        renderGroups[i].primitives[renderGroups[i].numPrimitives++] = (int)primitives.size()-1;

                        //Update render group buffer
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderGroupsBuffer);
                        glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(RenderGroup), sizeof(RenderGroup), &renderGroups[i]); 
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);     

                        //Update primitives buffer
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
                        glBufferData(GL_SHADER_STORAGE_BUFFER, primitives.size() * sizeof(Primitive), primitives.data(),  GL_STATIC_READ); 
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);             

                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }                    
                
                if (ImGui::IsItemClicked())
                    clickedGroup = i;
                
                //If the group is open, render its children
                if (groupOpen)
                {
                    ImGui::TreePush();
                    for(int j=0; j<renderGroups[i].numPrimitives; j++)
                    {
                        const bool isPrimitiveSelected = (selectedPrimitiveInGroup == j);
                        
                        std::string label = "Primitive " + std::to_string(j);
                        if (ImGui::Selectable(label.c_str(), isPrimitiveSelected))
                        {
                            selectedPrimitiveInGroup = j;
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

    if(selectedGroup >=0 && selectedPrimitiveInGroup>=0 ){
        
        //Guizmo
        int selectedPrimitive = renderGroups[selectedGroup].primitives[selectedPrimitiveInGroup];
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        glm::mat4 modelMatrix = glm::inverse(primitives[selectedPrimitive].transform);
        bool changed = ImGuizmo::Manipulate(&cam.GetViewMatrix()[0][0], &cam.GetProjectionMatrix()[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &modelMatrix[0][0], NULL, NULL);    
        if(changed)
        {
            glm::mat4 invMat = glm::inverse(modelMatrix);
            primitives[selectedPrimitive].transform = invMat;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, selectedPrimitive * sizeof(Primitive) + offsetof(Primitive, transform), sizeof(glm::mat4), &invMat); 
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);      
        }

        
        if(ImGui::CollapsingHeader("Group Properties"))
        {
            int opType = (int)renderGroups[selectedGroup].opType;
            bool updateRenderGroupData=false;
            if (ImGui::Combo("Medium Type", &opType, "Classic\0Smooth\0"))
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
        
        if(ImGui::CollapsingHeader("Primitive Properties"))
        {
            Primitive *primitive = &primitives[selectedPrimitive];
            bool updatePrimitiveData=false;
            if(primitive->Type == PrimitiveType::Sphere)
            {
                updatePrimitiveData |= ImGui::SliderFloat("Radius", &primitive->Data1.x, 0, 10);
            }
            else if(primitive->Type == PrimitiveType::Box)
            {
                updatePrimitiveData |= ImGui::SliderFloat3("Size", &primitive->Data1.x, 0, 10);
            }

            if(updatePrimitiveData)
            {
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, selectedPrimitive * sizeof(Primitive) + offsetof(Primitive, Data1), sizeof(glm::vec4) + sizeof(glm::vec2), &primitive->Data1[0]); 
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            }
        }
    }
    
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
