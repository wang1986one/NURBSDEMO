cmake_minimum_required(VERSION 3.19)
project(utils)

set(TARGET utils)

set(PUBLIC_HDR_DIR include)

# ==================================================================================================
# Sources and headers
# ==================================================================================================
file(GLOB_RECURSE PUBLIC_HDRS ${PUBLIC_HDR_DIR}/${TARGET}/*.h)

set(SRCS
        src/api_level.cpp
        src/architecture.cpp
        src/ashmem.cpp
        src/debug.cpp
        src/Allocator.cpp
        src/CallStack.cpp
        src/CString.cpp
        src/CountDownLatch.cpp
        src/CyclicBarrier.cpp
        src/EntityManager.cpp
        src/EntityManagerImpl.h
        src/JobSystem.cpp
        src/Log.cpp
        src/NameComponentManager.cpp
        src/ostream.cpp
        src/Panic.cpp
        src/Path.cpp
        src/Profiler.cpp
        src/sstream.cpp
        src/string.cpp
        src/ThreadUtils.cpp
)

if (WIN32)
    list(APPEND SRCS src/win32/Path.cpp)
endif()

if (WEBGL)
    list(APPEND SRCS src/web/Path.cpp)
endif()

# ==================================================================================================
# Includes and target definition
# ==================================================================================================
include_directories(${PUBLIC_HDR_DIR})

add_library(${TARGET} STATIC ${PUBLIC_HDRS} ${SRCS})
target_include_directories(${TARGET} PUBLIC ${PUBLIC_HDR_DIR})
set_target_properties(${TARGET} PROPERTIES FOLDER Libs)
target_link_libraries(${TARGET} PUBLIC tsl)



if (WIN32)
    # Needed for shlwapi.h (GetModuleFileName)
    target_link_libraries(${TARGET} PUBLIC Shlwapi)
endif()









