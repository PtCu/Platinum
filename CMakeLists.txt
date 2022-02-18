cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME "platinum")
project (${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# 开启工程虚拟目录
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "settle from environ VCPKG_ROOT")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
endif()

find_package(glm CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

find_package(TBB CONFIG REQUIRED)

find_package(GTest CONFIG REQUIRED)

# glog configuration
find_package(glog CONFIG REQUIRED)
set(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
if(WIN32)
  add_definitions( -D GOOGLE_GLOG_DLL_DECL= )
endif()

add_definitions( -D HAVE_SNPRINTF)
add_definitions(-D NOMINMAX)
# Boost header only
find_package(Boost 1.77.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()


#设置头文件目录
include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/include
	${CMAKE_BINARY_DIR}/config
)


configure_file(config/ROOT_PATH.h.in config/ROOT_PATH.h)

# 收集所有头文件源文件，存入 ALL_SOURCE 变量中
file ( GLOB ALL_SOURCE
  src/core/*
  src/accelerator/*
  src/camera/*
  src/filter/*
  src/integrator/*
  src/light/*
  src/material/*
  src/material/bxdf/*
  src/math/*
  src/sampler/*
  src/shape/*
  src/texture/*
)

# 定义按目录结构归类的方法
function(assign_source_group)
    foreach (_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
			# 获取相对路径
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else ()
            set(_source_rel "${_source}")
        endif ()
		#取得文件目录。如src/core
        get_filename_component(_source_path "${_source_rel}" PATH)
		# 替换\\为/，并将结果保存在_source_path_msvc
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
		message("Group ${_source} into ${_source_path_msvc}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach ()
endfunction(assign_source_group)

foreach (_source IN ITEMS ${ALL_SOURCE})
    assign_source_group(${_source})
endforeach ()


# if(MSVC)
# 	add_compile_options(/O2 /fp:fast /arch:AVX2)
# endif()

# if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
#     set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0")
#     message("Debug mode:${CMAKE_C_FLAGS_DEBUG}")
# elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
#     if(MSVC)
#         set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /fp:fast /arch:AVX2")
#     endif()    
#     message("Release mode:${CMAKE_C_FLAGS_RELEASE}")
# else()
#     set (CMAKE_BUILD_TYPE "RelWithDebInfo")
#     message (
#         STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
#     )
# endif()

add_library(${PROJECT_NAME} STATIC ${ALL_SOURCE})

set(EXTRA_LIBS ${EXTRA_LIBS} glm::glm assimp::assimp TBB::tbb glog::glog GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

set(ALL_LIBS ${PROJECT_NAME} ${EXTRA_LIBS})

add_executable(${PROJECT_NAME}_exe src/main/main.cpp)
set_target_properties(${PROJECT_NAME}_exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_exe PROPERTIES LINK_FLAGS /WHOLEARCHIVE:${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_exe ${ALL_LIBS})


# 单元测试
file(GLOB TEST_SOURCE src/tests/*.cpp)
add_executable(${PROJECT_NAME}_test ${TEST_SOURCE})
target_link_libraries(${PROJECT_NAME}_test ${ALL_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

