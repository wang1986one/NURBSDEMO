set(TARGET_NAME filamentdemo)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE ${TARGET_NAME}_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE ${TARGET_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${${TARGET_NAME}_HEAD} ${${TARGET_NAME}_SRC})
add_executable(${TARGET_NAME} ${${TARGET_NAME}_HEAD} ${${TARGET_NAME}_SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${EIGEN3_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC glad)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC tinyxml2)
target_link_libraries(${TARGET_NAME} PUBLIC filament)
target_link_libraries(${TARGET_NAME} PUBLIC sdl2)
target_link_libraries(${TARGET_NAME} PRIVATE sample-resources)
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")



