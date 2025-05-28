file(
	GLOB_RECURSE
	GearX_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/core/framework/component/conponent.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/core/framework/component/conponent.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/object_wrapper.cpp
    PROPERTIES COMPILE_FLAGS "/bigobj"
)

set(
	IMGUI_SOURCE
	"${DEPENCIES_DIR}/imgui/imgui.cpp"
    "${DEPENCIES_DIR}/imgui/imgui_draw.cpp"
    "${DEPENCIES_DIR}/imgui/imgui_tables.cpp"
    "${DEPENCIES_DIR}/imgui/imgui_widgets.cpp"
    "${DEPENCIES_DIR}/imgui/imgui_demo.cpp"
    "${DEPENCIES_DIR}/imgui/backends/imgui_impl_sdl3.cpp"
    "${DEPENCIES_DIR}/imgui/backends/imgui_impl_sdlrenderer3.cpp"
)
add_library(
	imgui STATIC
	${IMGUI_SOURCE}
)
add_library(
	runtime STATIC
	${GearX_SOURCES}
)
target_include_directories(
	runtime PUBLIC 
	"./"
)
target_link_libraries(
	runtime
	PUBLIC
	# List libraries link to the target library
	imgui
	SDL3
	SDL3_image
	SDL3_ttf
	SDL3_mixer
    SDL3_gfx_Static
	cereal::cereal
	rttr_core
	box2d
	lua54
	sol2
)



