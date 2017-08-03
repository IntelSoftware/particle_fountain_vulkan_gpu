cmake_minimum_required (VERSION 2.6)
project(psim_cpu)


set(GLM_DIR "" CACHE PATH "")
set(GLFW_DIR "" CACHE PATH "")
set(VULKAN_DIR "" CACHE PATH "")

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_custom_command(
    OUTPUT vert.spv
    COMMAND ${VULKAN_DIR}/Bin/glslangValidator.exe -V ${CMAKE_CURRENT_SOURCE_DIR}/renderer/shaders/particle_fountain.vert
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(
    OUTPUT frag.spv
    COMMAND ${VULKAN_DIR}/Bin/glslangValidator.exe -V ${CMAKE_CURRENT_SOURCE_DIR}/renderer/shaders/particle_fountain.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    BuildShaders ALL
    DEPENDS vert.spv frag.spv
)

include_directories(
    particlesim/include
    renderer/include 
    ${GLM_DIR}
    ${GLFW_DIR}/include
    ${VULKAN_DIR}/include
)
link_directories(
    ${GLFW_DIR}/lib-vc2015
    ${VULKAN_DIR}/Lib
)

set(STATIC_LIBS glfw3.lib vulkan-1.lib)

file(GLOB SOURCES "particlesim/src/*.cpp" "renderer/src/*.cpp" main.cpp)

add_executable(psim_cpu ${SOURCES})
target_link_libraries(psim_cpu ${STATIC_LIBS})
