cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested to build this target.")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Boolean describing whether the value of CXX_STANDARD is a requirement.")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Boolean specifying whether compiler specific extensions are requested.")

project(FE3tilesets)

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(FATAL_ERROR "This project can only be built for Windows.")
endif()

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/snes/*.cpp")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/snes/*.h")
file(GLOB BMP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/winbmp/winbmp.*")
file(GLOB MAGICK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/magick/magick.*")
file(GLOB LUNAR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/DLLcode/LunarDLL.*")

add_executable(main main.cpp ${SOURCE_FILES} ${MAGICK_FILES} ${BMP_FILES} ${HEADER_FILES} ${LUNAR_FILES})

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(winbmp/)
include_directories(magick/)
include_directories(snes/)
include_directories(DLLcode/)
target_link_libraries(main ${ImageMagick_LIBRARIES} gdi32)
target_link_options(main PRIVATE -static)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/dlls/Lunar Compress.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/Lunar Compress.dll"
)