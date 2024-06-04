find_package(OpenGL REQUIRED)


add_library(glm INTERFACE)
add_library(khr INTERFACE)
add_library(glad INTERFACE)
add_library(stb_image INTERFACE)

add_subdirectory(GLFW EXCLUDE_FROM_ALL)
add_subdirectory(assimp EXCLUDE_FROM_ALL)


target_include_directories(glm INTERFACE glm)
target_include_directories(khr INTERFACE KHR)
target_include_directories(glad INTERFACE GLAD)
target_include_directories(stb_image INTERFACE stb_image)

#add_library(sdl2 INTERFACE)

#target_include_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Include/SDL2 )
#target_link_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Lib )

#target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)

add_library(imgui STATIC)

target_include_directories(imgui PUBLIC imgui)

target_sources(imgui PRIVATE 
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp

    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3_loader.h
 
)

target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES} glfw)

