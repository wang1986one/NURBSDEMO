set(lib_folder "Extern")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET std_image)
    set(std_image_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/stb_Image)
    file(GLOB_RECURSE std_image_impl CONFIGURE_DEPENDS  
    "${std_image_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE std_image_head CONFIGURE_DEPENDS  
    "${std_image_SOURCE_DIR_}/*.h")
    source_group(TREE "${std_image_SOURCE_DIR_}" FILES ${std_image_head} ${std_image_impl})
    add_library(std_image STATIC ${std_image_head} ${std_image_impl})
    set_target_properties(std_image PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(std_image PUBLIC $<BUILD_INTERFACE:${std_image_SOURCE_DIR_}/include>)
    set_target_properties(std_image PROPERTIES FOLDER ${lib_folder}/std_image)
endif()

if(NOT TARGET imgui)
    set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR_}/IconsForkAwesome.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3_loader.h")
    set(_imgui_node_editor_Sources
    ${imgui_SOURCE_DIR_}/crude_json.cpp
    ${imgui_SOURCE_DIR_}/crude_json.h
    ${imgui_SOURCE_DIR_}/imgui_bezier_math.h
    ${imgui_SOURCE_DIR_}/imgui_bezier_math.inl
    ${imgui_SOURCE_DIR_}/imgui_canvas.cpp
    ${imgui_SOURCE_DIR_}/imgui_canvas.cpp
    ${imgui_SOURCE_DIR_}/imgui_canvas.h
    ${imgui_SOURCE_DIR_}/imgui_canvas.h
    ${imgui_SOURCE_DIR_}/imgui_extra_math.h
    ${imgui_SOURCE_DIR_}/imgui_extra_math.inl
    ${imgui_SOURCE_DIR_}/imgui_node_editor_api.cpp
    ${imgui_SOURCE_DIR_}/imgui_node_editor_internal.h
    ${imgui_SOURCE_DIR_}/imgui_node_editor_internal.inl
    ${imgui_SOURCE_DIR_}/imgui_node_editor.cpp
    ${imgui_SOURCE_DIR_}/imgui_node_editor.h
    ${imgui_SOURCE_DIR_}/imgui_node_editor.natvis
)
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl} ${_imgui_node_editor_Sources})
    target_link_libraries(imgui PUBLIC glfw)
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
    set_target_properties(imgui PROPERTIES FOLDER ${lib_folder}/imgui)
endif()
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${lib_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${lib_folder}/glfw)
endif()
if(NOT TARGET glad)
    set(glad_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/glad)
    file(GLOB_RECURSE glad_impl CONFIGURE_DEPENDS  
    "${glad_SOURCE_DIR_}/*.c")
    file(GLOB_RECURSE glad_head CONFIGURE_DEPENDS  
    "${glad_SOURCE_DIR_}/*.h")
    add_library(glad STATIC ${glad_impl} ${glad_head})
    target_include_directories(glad PUBLIC $<BUILD_INTERFACE:${glad_SOURCE_DIR_}>)
    set_target_properties(glad PROPERTIES FOLDER ${lib_folder}/glad)
endif()

if(NOT TARGET tinyxml2)
    set(tinyxml2_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
    file(GLOB_RECURSE tinyxml2_impl CONFIGURE_DEPENDS  
    "${tinyxml2_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE tinyxml2_head CONFIGURE_DEPENDS  
    "${tinyxml2_SOURCE_DIR_}/*.h")
    add_library(tinyxml2 STATIC ${tinyxml2_impl} ${tinyxml2_head})
    target_include_directories(tinyxml2 PUBLIC $<BUILD_INTERFACE:${tinyxml2_SOURCE_DIR_}>)
    set_target_properties(tinyxml2 PROPERTIES FOLDER ${lib_folder}/tinyxml2)
endif()


if(NOT TARGET igl)
    set(igl_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/igl)
    file(GLOB_RECURSE igl_impl CONFIGURE_DEPENDS  
    "${igl_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE igl_head CONFIGURE_DEPENDS  
    "${igl_SOURCE_DIR_}/*.h")
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${igl_head} ${igl_impl})

    add_library(igl STATIC ${igl_impl} ${igl_head})
    target_link_libraries(igl PUBLIC imgui)
    target_link_libraries(igl PUBLIC std_image)
    target_link_libraries(igl PUBLIC glad)
    target_link_libraries(igl PUBLIC glfw)
    target_link_libraries(igl PUBLIC tinyxml2)
    target_include_directories(igl PUBLIC ${EIGEN3_INCLUDE_DIR})
    target_include_directories(igl PUBLIC $<BUILD_INTERFACE:${igl_SOURCE_DIR_}>)
    set_target_properties(igl PROPERTIES FOLDER ${lib_folder}/igl)
    target_compile_options(igl PRIVATE /bigobj )
endif()
add_subdirectory(filament)







