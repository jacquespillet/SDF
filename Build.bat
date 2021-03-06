@echo off


REM IMGUI
set dearimgui= ..\ext\dearimgui\
set dearimguiSrc=%dearimgui%imgui_impl_opengl3.cpp %dearimgui%imgui_impl_glfw.cpp %dearimgui%imgui.cpp %dearimgui%imgui_demo.cpp %dearimgui%imgui_draw.cpp %dearimgui%imgui_widgets.cpp
set dearimguiObj=%dearimgui%\build\imgui_impl_opengl3.obj %dearimgui%\build\imgui_impl_glfw.obj %dearimgui%\build\imgui.obj %dearimgui%\build\imgui_demo.obj %dearimgui%\build\imgui_draw.obj %dearimgui%\build\imgui_widgets.obj
REM --------------------

REM IMGUI
set imGuizmo= ..\ext\imGuizmo\
set imGuizmoSrc=%imGuizmo%ImGuizmo.cpp
set imGuizmoInclude=%imGuizmo%
REM --------------------

REM GLEW
set glewLib=..\ext\glew\lib\Release\x64\glew32.lib
set glewIncludes=..\ext\glew\include
set glewBin= C:\Users\jacqu\OneDrive\Documents\Boulot\2020\Exp\ext\glew\bin\Release\x64\glew32.dll
REM --------------------

REM GLFW
set glfwInclude= ..\ext\glfw_\include
set glfwLib= ..\ext\glfw_\lib\glfw3.lib
REM --------------------

REM GLM
set glmInclude= ..\ext\glm
REM --------------------

REM SRC FILES
set GL_srcFiles= ../src/GL_Helpers/GL_Mesh.cpp  ../src/GL_Helpers/GL_Shader.cpp ../src/GL_Helpers/Util.cpp ../src/GL_Helpers/GL_Camera.cpp ../src/GL_Helpers/GL_Material.cpp
set demoFiles= ../src/Demos/Template/Template.cpp
REM --------------------


set compilerFlags= /DEBUG -MP -MT -nologo -EHa- -Od -Oi -W4 -Z7 -EHsc -wd4201 -wd4310 -wd4100  -wd4996 /I ../src /I ..\ext\stb /I %dearimgui% /I %imGuizmoInclude% /I %glfwInclude%  /I %glewIncludes%  /I %glmInclude% /I %assimpIncludes%
set linkerFlags=  -opt:ref Gdi32.lib Shell32.lib opengl32.lib %dearimguiObj% %glfwLib%  %glewLib% %assimpLib%

IF NOT EXIST .\build mkdir .\build
pushd .\build
cl.exe %compilerFlags% ..\Main.cpp %imGuizmoSrc% %demoFiles% %GL_srcFiles% /link %linkerFlags%

copy %glewBin% .\ >nul
copy %assimpBin% .\ >nul

IF NOT EXIST .\shaders mkdir .\shaders
xcopy ..\shaders .\shaders\ /s /e /Y  >nul

IF NOT EXIST .\resources mkdir .\resources
xcopy ..\resources .\resources\  /s /e /Y /D >nul

popd