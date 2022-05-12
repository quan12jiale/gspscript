##设置库名称
set(LIBRARY_TARGET_NAME dbscript)

##查找所有头文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_HEADER_FILES
    LIST_DIRECTORIES False 
    "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/*.h"
)
##设置VS筛选器，头文件分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}"
    PREFIX "Header Files"
    FILES ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
)

##查找所有源文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_SRC_FILES
    LIST_DIRECTORIES False 
    "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/*.cpp"
)
##设置VS筛选器，源码分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}"
    PREFIX "Source Files"
    FILES ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)


##设置生成目标
add_executable(${LIBRARY_TARGET_NAME} WIN32
    ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
    ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)

##指定工程文件分组
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES FOLDER "FolderName")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE") ## /SUBSYSTEM:WINDOWS

set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES LINK_FLAGS "/SAFESEH:NO /LARGEADDRESSAWARE")


##指定工程生成文件组织结构
#set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES FOLDER ${FOLDER_NAME})

##打开qt特性配置
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOMOC ON)

##设置预处理器定义
## _UNICODE宏是因为libxl.h判断如果定义了_UNICODE宏，会使用wchar_t;否则使用char。使用char会导致中文乱码
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE UNICODE _UNICODE WIN32 QT_DLL QT_NO_DEBUG NDEBUG QT_CORE_LIB) #  QT_GUI_LIB QT_WIDGETS_LIB

##配置构建/使用时的头文件路径
target_include_directories(
    ${LIBRARY_TARGET_NAME}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/>"
)

##配置库依赖
find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)
find_package(GLDRS COMPONENTS libxl REQUIRED)
find_package(GBIM COMPONENTS Common REQUIRED)
find_package(GDB COMPONENTS GGDB REQUIRED)

target_link_libraries(${LIBRARY_TARGET_NAME}
	PRIVATE GLDRS::libxl
    PRIVATE Qt5::Core Qt5::Network Qt5::Sql
	PRIVATE GBIM::Common
	PRIVATE GDB::GGDB
)
