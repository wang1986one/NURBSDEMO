set(TARGET_NAME Geomerty)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE Geomerty_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE Geomerty_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${Geomerty_HEAD} ${Geomerty_SRC})
add_executable(${TARGET_NAME} ${Geomerty_HEAD} ${Geomerty_SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${EIGEN3_INCLUDE_DIR})
# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC glad)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC tinyxml2)
target_link_libraries(${TARGET_NAME} PUBLIC igl)
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)



