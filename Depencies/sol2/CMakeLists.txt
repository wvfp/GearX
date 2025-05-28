# 添加sol2库（头文件库）
add_library(sol2 INTERFACE)
target_include_directories(sol2 
    INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# 链接Lua依赖
target_link_libraries(sol2 INTERFACE lua54)