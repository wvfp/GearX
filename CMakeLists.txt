cmake_minimum_required(VERSION 3.22)

project(GearX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GEARX_BIN_DIR ${CMAKE_BINARY_DIR})
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "/Ox")
endif()
set(DEPENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Depencies)
set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Depencies/SDL3/cmake")
include(Depencies/SDL3/cmake/SDL3Config.cmake)
include(Depencies/SDL3/cmake/SDL3_imageConfig.cmake)
include(Depencies/SDL3/cmake/SDL3_ttfConfig.cmake)
include_directories(
    Depencies/SDL3/include
    Depencies/lua/src
    Depencies/sol2/include
    Depencies/imgui/
    Depencies/imgui/backends
    Depencies/
    
)
link_directories(
   Depencies/SDL3/lib/x64/
)
add_subdirectory(Depencies/cereal)
add_subdirectory(Depencies/box2d)
add_subdirectory(Depencies/SDL3_gfx)
add_subdirectory(Depencies/lua)
add_subdirectory(Depencies/sol2)
add_subdirectory(Depencies/rttr)
add_subdirectory(GearX)

add_executable(GearXEngine
 "main.cpp")
target_link_libraries(GearXEngine PRIVATE
    Engine
)
target_link_directories(
    GearXEngine PRIVATE
    Depencies/SDL3/lib/x64
)

add_executable(GearXStarter
 "starter.cpp")
target_link_libraries(GearXStarter PRIVATE
    Starter
)
target_link_directories(
    GearXStarter PRIVATE
    Depencies/SDL3/lib/x64
)

file(COPY Depencies/SDL3/lib/x64/ DESTINATION ${GEARX_BIN_DIR} 
    PATTERN "*.dll"
    PATTERN "*.pdb" EXCLUDE
    PATTERN "*.lib" EXCLUDE
  )

