cmake_minimum_required(VERSION 3.20)

project(file)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-O3 -Wall -Wpedantic -Wextra -Wformat=2 -Wshadow -Wcast-align -Wconversion -Wfloat-equal -Werror=shadow) # 自定义编译选项

aux_source_directory(./src SRC_DIR) # 添加源代码文件夹, 自动扫描所有文件

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) # 可执行文件输出目录

# 使用pkg-config
find_package(PkgConfig REQUIRED) # 查找PkgConfig

pkg_check_modules(GTKMM gtkmm-4.0) # 查找Gtkmm
pkg_check_modules(MAGICKCXX REQUIRED Magick++) #查找Magick++

link_directories(${GTKMM_LIBRARY_DIRS}) # gtkmm链接库文件
link_directories(${MAGICKCXX_LIBRARY_DIRS}) #Magick++链接文件库

include_directories(${GTKMM_INCLUDE_DIRS}) # gtkmm头文件
include_directories(${MAGICKCXX_INCLUDE_DIRS}) # Magick++头文件

# 使用glib-compile-resources
find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED) # 查找glib-compile-resources

# 设置源文件
set(GRESOURCE_C ${CMAKE_CURRENT_SOURCE_DIR}/src/gresource.c)
set(GRESOURCE_XML ${CMAKE_CURRENT_SOURCE_DIR}/data/gresource.xml)

# 设置编译命令
add_custom_command(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/ # 编译命令工作目录
    OUTPUT ${GRESOURCE_C}                               # 输出文件
    COMMAND ${GLIB_COMPILE_RESOURCES}                   # 命令行
    ARGS
        --target=${GRESOURCE_C}
        --generate-source
        ${GRESOURCE_XML}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
    DEPENDS                                             # 必须文件
        ${CMAKE_CURRENT_SOURCE_DIR}/data/window.ui
)

# 设置自定义目标
add_custom_target(
    gresource
    DEPENDS ${GRESOURCE_C}
)

# 编译
add_executable(
    ${PROJECT_NAME} 
    ${SRC_DIR} 
    ${GRESOURCE_C}
) # 编译

# 设置资源文件属性:"这是在生成MakeFile之后生成的"
set_source_files_properties(
    ${GRESOURCE_C}
    PROPERTIES GENERATED TRUE
)

add_dependencies(${PROJECT_NAME} gresource) # 添加依赖

#添加库的编译选项
target_compile_options(
    ${PROJECT_NAME} PUBLIC 
    ${MAGICKCXX_CFLAGS_OTHER} 
    -DGDK_DISABLE_DEPRECATED 
    -DGTK_DISABLE_DEPRECATED
) #添加库的编译选项

# 链接
target_link_libraries(
    ${PROJECT_NAME} 
    ${GTKMM_LIBRARIES} 
    ${MAGICKCXX_LIBRARIES}
)# 链接
