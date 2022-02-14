cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME "Platinum")
project (${PROJECT_NAME} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# 开启工程虚拟目录
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(glm CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

find_package(TBB CONFIG REQUIRED)

# glog configuration
find_package(glog CONFIG REQUIRED)
set(WITH_GFLAGS OFF CACHE BOOL "Use gflags")
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
if(WIN32)
  add_definitions( -D GOOGLE_GLOG_DLL_DECL= )
endif()

add_definitions( -D HAVE_SNPRINTF)

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

# 收集所有头文件源文件，存入 ALL_FILES 变量中
file(GLOB_RECURSE ALL_FILES "src/*.h*" "src/*.c*")

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

foreach (_source IN ITEMS ${ALL_FILES})
    assign_source_group(${_source})
endforeach ()

set(EXTRA_LIBS ${EXTRA_LIBS} glm::glm assimp::assimp TBB::tbb glog::glog)

if(MSVC)
	add_compile_options(/O2 /fp:fast /arch:AVX2)
endif()

configure_file(config/ROOT_PATH.h.in config/ROOT_PATH.h)

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

