cmake_minimum_required(VERSION 3.18)
project(lua VERSION 5.4 LANGUAGES C)

include_directories("${PROJECT_BINARY_DIR}")
 
aux_source_directory(./src C_LIST)			# 将当前路径下的所有源文件放在C_LIST变量中
list(REMOVE_ITEM C_LIST "./src/lua.c" "./src/luac.c")	# 从C_LIST中删除"./lua.c"和"./luac.c"

file(GLOB_RECURSE luamain RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/lua*.c") 	# 得到当前目录下所有以lua开头的源文件，即lua.c和luac.c
add_library(lua54 STATIC ${C_LIST})
#foreach(mainfile IN LISTS luamain)
#    get_filename_component(mainname ${mainfile} NAME_WE)	# 得到文件名lua和luac，
#    add_executable(${mainname} ${mainfile} ${C_LIST})	# mainname为可执行文件的名字，mainfile C_LIST则是生成可执行文件的源文件。
#endforeach()