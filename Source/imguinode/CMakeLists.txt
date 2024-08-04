set(TARGET_NAME Imgui_node)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE Imgui_node_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE Imgui_node_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${Imgui_node_HEAD} ${Imgui_node_SRC})
add_executable(${TARGET_NAME} ${Imgui_node_HEAD} ${Imgui_node_SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${EIGEN3_INCLUDE_DIR})
# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC glad)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)

file(GLOB _ResourceFont ${CMAKE_CURRENT_SOURCE_DIR}/*.ttf)
foreach(_Font "${_ResourceFont}" )
    message("font is ${_Font}")
    add_custom_command(
        TARGET ${TARGET_NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Font} ${CMAKE_CURRENT_BINARY_DIR}
    )

endforeach()



