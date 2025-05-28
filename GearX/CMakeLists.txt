file(COPY "./asset" DESTINATION ${GEARX_BIN_DIR})
file(COPY "./example" DESTINATION ${GEARX_BIN_DIR})

add_subdirectory(runtime)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/runtime/)
add_library(Engine 
	engine.cpp engine.hpp)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(Engine runtime)

add_library(Starter
	starter.cpp starter.hpp)
target_include_directories(Starter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(Starter runtime)
