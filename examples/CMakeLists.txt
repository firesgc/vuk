cmake_minimum_required(VERSION 3.7)
project(vuk-examples)

add_subdirectory(../ext/vk-bootstrap vk-bootstrap)
set(GLFW_BUILD_DOCS FALSE)
set(GLFW_INSTALL FALSE)
add_subdirectory(../ext/glfw glfw)
add_subdirectory(../ext/glm glm)

SET(imgui_sources ../ext/imgui/imgui.cpp ../ext/imgui/imgui_draw.cpp ../ext/imgui/imgui_demo.cpp ../ext/imgui/imgui_widgets.cpp ../ext/imgui/examples/imgui_impl_glfw.cpp)

add_executable(vuk_all_examples)
target_include_directories(vuk_all_examples SYSTEM PRIVATE ../ext/stb ../ext/imgui)
target_sources(vuk_all_examples PRIVATE example_browser.cpp imgui.cpp stbi.cpp ${imgui_sources})
target_compile_definitions(vuk_all_examples PRIVATE GLM_FORCE_SIZE_FUNC GLM_FORCE_EXPLICIT_CTOR GLM_ENABLE_EXPERIMENTAL GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_link_libraries(vuk_all_examples PRIVATE vuk)
target_link_libraries_system(vuk_all_examples PRIVATE vk-bootstrap glfw glm)

function(ADD_EXAMPLE name)
    set(FULL_NAME "vuk_example_${name}")
    add_executable(${FULL_NAME})
    target_sources(${FULL_NAME} PRIVATE "${name}.cpp" example_runner_single.cpp imgui.cpp stbi.cpp ${imgui_sources})
    target_sources(vuk_all_examples PRIVATE "${name}.cpp")
    target_include_directories(${FULL_NAME} SYSTEM PRIVATE ../ext/stb ../ext/imgui)
    target_compile_definitions(${FULL_NAME} PRIVATE GLM_FORCE_SIZE_FUNC GLM_FORCE_EXPLICIT_CTOR GLM_ENABLE_EXPERIMENTAL GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZERO_TO_ONE)
    target_link_libraries(${FULL_NAME} PRIVATE vuk)
    target_link_libraries_system(${FULL_NAME} PRIVATE vk-bootstrap glfw glm)
endfunction(ADD_EXAMPLE)

ADD_EXAMPLE(01_triangle)
ADD_EXAMPLE(02_cube)
ADD_EXAMPLE(03_multipass)
ADD_EXAMPLE(04_texture)
ADD_EXAMPLE(05_deferred)
ADD_EXAMPLE(06_msaa)