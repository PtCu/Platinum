# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.15)

# 项目信息
set(PROJECT_NAME Platinum)
project(${PROJECT_NAME})

# 使用C++17特性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# 开启工程虚拟目录
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# glog
SET(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
IF(WIN32)
  ADD_DEFINITIONS( -D GOOGLE_GLOG_DLL_DECL= )
ENDIF()
ADD_DEFINITIONS( -D HAVE_SNPRINTF)
ADD_SUBDIRECTORY(src/ext/glog)
SET_PROPERTY(TARGET glog logging_unittest demangle_unittest utilities_unittest stl_logging_unittest signalhandler_unittest stacktrace_unittest symbolize_unittest PROPERTY FOLDER "ext")


INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/src/ext/glog/src")
INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/src/ext/header_only")
INCLUDE_DIRECTORIES ("${CMAKE_BINARY_DIR}/src/ext/glog")


file(GLOB ACCELECTORS ./src/accelerators/*.h ./src/accelerators/*.cpp)
source_group("accelectors" FILES ${ACCELECTORS})

file(GLOB CAMERAS ./src/cameras/*.h ./src/cameras/*.cpp)
source_group("cameras" FILES ${CAMERAS})

file(GLOB CORE ./src/core/*.h ./src/core/*.cpp)
source_group("core" FILES ${CORE})

file(GLOB FILTERS ./src/filters/*.h ./src/filters/*.cpp)
source_group("filters" FILES ${FILTERS})

file(GLOB INTEGRATORS ./src/integrators/*.h ./src/integrators/*.cpp)
source_group("integrators" FILES ${INTEGRATORS})

file(GLOB MATERIALS ./src/materials/*.h ./src/materials/*.cpp)
source_group("materials" FILES ${MATERIALS})

file(GLOB SAMPLERS ./src/math/*.h ./src/math/*.cpp)
source_group("math" FILES ${MATH})

file(GLOB SAMPLERS ./src/samplers/*.h ./src/samplers/*.cpp)
source_group("samplers" FILES ${SAMPLERS})

file(GLOB SHAPES ./src/shapes/*.h ./src/shapes/*.cpp)
source_group("shapes" FILES ${SHAPES})

file(GLOB LIGHTS ./src/lights/*.h ./src/lights/*.cpp)
source_group("lights" FILES ${LIGHTS})

file(GLOB TEXTURES ./src/textures/*.h ./src/textures/*.cpp)
source_group("textures" FILES ${TEXTURES})


file(GLOB SHAPES ./src/util/*.h ./src/util/*.cpp)
source_group("util" FILES ${UTIL})

file(GLOB SRCS ./src/*.cpp)

add_executable(${PROJECT_NAME} ${ACCELECTORS} ${CAMERAS} ${CORE} ${FILTERS} ${MATH} ${UTIL}
		 ${INTEGRATORS} ${MATERIALS} ${SAMPLERS} ${SHAPES} ${LIGHTS} ${TEXTURES} ${SRCS})

set(EXTRA_LIBS ${EXTRA_LIBS}  glog )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    #set_target_properties(${APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${APP_NAME}>)
    set_target_properties(${APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif ()