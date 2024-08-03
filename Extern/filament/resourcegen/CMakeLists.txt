cmake_minimum_required(VERSION 3.19)
project(filament-samples C ASM)

if (FILAMENT_SKIP_SAMPLES)
    return()
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GENERATION_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set(RESOURCE_DIR  "${GENERATION_ROOT}/generated/resources")
set(MATERIAL_DIR  "${GENERATION_ROOT}/generated/material")
set(TEXTURE_DIR "${GENERATION_ROOT}/generated/texture")
set(RESOURCE_BINS)

# ==================================================================================================
# Build materials
# ==================================================================================================

set(MATERIAL_SRCS
        materials/aiDefaultMat.mat
        materials/aiDefaultTrans.mat
        materials/depthVisualizer.mat
        materials/transparentColor.mat
        materials/bakedColor.mat
        materials/bakedTexture.mat
        materials/pointSprites.mat
        materials/aoPreview.mat
        materials/groundShadow.mat
        materials/heightfield.mat
        materials/image.mat
        materials/mirror.mat
        materials/overdraw.mat
        materials/sandboxCloth.mat
        materials/sandboxLit.mat
        materials/sandboxLitFade.mat
        materials/sandboxLitTransparent.mat
        materials/sandboxLitThinRefraction.mat
        materials/sandboxLitThinRefractionSsr.mat
        materials/sandboxLitSolidRefraction.mat
        materials/sandboxLitSolidRefractionSsr.mat
        materials/sandboxSpecGloss.mat
        materials/sandboxSubsurface.mat
        materials/sandboxUnlit.mat
        materials/texturedLit.mat
)

if (CMAKE_CROSSCOMPILING)
    include(${IMPORT_EXECUTABLES})
endif()

file(MAKE_DIRECTORY ${MATERIAL_DIR})

set (MATC_FLAGS ${MATC_BASE_FLAGS})
if (FILAMENT_SAMPLES_STEREO_TYPE STREQUAL "instanced")
    set (MATC_FLAGS ${MATC_FLAGS} -PstereoscopicType=instanced)
    add_definitions(-DFILAMENT_SAMPLES_STEREO_TYPE_INSTANCED)
elseif (FILAMENT_SAMPLES_STEREO_TYPE STREQUAL "multiview")
    set (MATC_FLAGS ${MATC_FLAGS} -PstereoscopicType=multiview)
    add_definitions(-DFILAMENT_SAMPLES_STEREO_TYPE_MULTIVIEW)
endif ()

foreach (mat_src ${MATERIAL_SRCS})
    get_filename_component(localname "${mat_src}" NAME_WE)
    get_filename_component(fullname "${mat_src}" ABSOLUTE)
    set(output_path "${MATERIAL_DIR}/${localname}.filamat")
    add_custom_command(
            OUTPUT ${output_path}
            COMMAND matc ${MATC_FLAGS} -o ${output_path} ${fullname}
            MAIN_DEPENDENCY ${mat_src}
            DEPENDS matc
            COMMENT "Compiling material ${mat_src} to ${output_path}"
    )
    list(APPEND RESOURCE_BINS ${output_path})
endforeach()

# ==================================================================================================
# Build sample resources
# ==================================================================================================

file(MAKE_DIRECTORY ${RESOURCE_DIR})

get_resgen_vars(${RESOURCE_DIR} resources)

add_custom_command(
        OUTPUT ${RESGEN_OUTPUTS}
        COMMAND resgen ${RESGEN_FLAGS} ${RESOURCE_BINS}
        DEPENDS resgen ${RESOURCE_BINS}
        COMMENT "Aggregating resources"
)

if (DEFINED RESGEN_SOURCE_FLAGS)
    set_source_files_properties(${RESGEN_SOURCE} PROPERTIES COMPILE_FLAGS ${RESGEN_SOURCE_FLAGS})
endif()

# CMake fails to invoke ar on Windows unless there is at least one C/C++ file in the library.
set(DUMMY_SRC "${RESOURCE_DIR}/dummy.c")
add_custom_command(OUTPUT ${DUMMY_SRC} COMMAND echo "//" > ${DUMMY_SRC})

add_library(sample-resources ${DUMMY_SRC} ${RESGEN_SOURCE})
target_include_directories(sample-resources PUBLIC $<BUILD_INTERFACE:${GENERATION_ROOT}>)
set_target_properties(sample-resources PROPERTIES FOLDER Filament/Resources)




