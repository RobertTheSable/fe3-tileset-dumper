cmake_minimum_required(VERSION 3.8)
project(FE3tilesets LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested to build this target.")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Boolean describing whether the value of CXX_STANDARD is a requirement.")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Boolean specifying whether compiler specific extensions are requested.")

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(FATAL_ERROR "This project can only be built for Windows.")
endif()

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/snes/*.cpp")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/snes/*.h")
file(GLOB MAGICK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/magick/magick.*")
file(GLOB LUNAR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/DLLcode/LunarDLL.*")

add_executable(main main.cpp ${SOURCE_FILES} ${MAGICK_FILES} ${HEADER_FILES} ${LUNAR_FILES})

if(NOT DEFINED ImageMagick_LIBRARIES)
    find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickCore )
    string(REGEX MATCH "[Qq](16|8)(HDRI)?" MAGICK_QUANTUM ${ImageMagick_LIBRARIES})
    if(${CMAKE_MATCH_COUNT} GREATER_EQUAL "1")
        if (NOT DEFINED MAGICK_QUANTUM_DEPTH)
            if(${CMAKE_MATCH_1} EQUAL "16")
                message(STATUS "Setting Quantum depth to 16.")
                set(MAGICK_QUANTUM_DEPTH "16")
            elseif(${CMAKE_MATCH_1} EQUAL "8")
                message(STATUS "Setting Quantum depth to 8.")
                set(MAGICK_QUANTUM_DEPTH "8")
            else()
                message(SEND_ERROR "Couldn't determine quantum depth, please define it manually.")
            endif()
        endif()
        if (NOT DEFINED MAGICK_HDRI_ENABLE)
            if(${CMAKE_MATCH_COUNT} EQUAL "2")
                message(STATUS "Enabling HDRI")
                set(MAGICK_HDRI_ENABLE "1")
            else()
                message(STATUS "Disabling HDRI")
                set(MAGICK_HDRI_ENABLE "0")
            endif()
        endif()
    endif()
endif()

include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(winbmp/)
include_directories(magick/)
include_directories(snes/)
include_directories(DLLcode/)
target_link_libraries(main ${ImageMagick_LIBRARIES})
target_compile_definitions(main PUBLIC
    MAGICKCORE_QUANTUM_DEPTH=${MAGICK_QUANTUM_DEPTH}
    MAGICKCORE_HDRI_ENABLE=${MAGICK_HDRI_ENABLE}
)
if(DEFINED MINGW) 
    message(STATUS "Adding MINGW static flag.")
    target_link_options(main PRIVATE -static)
endif()

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/dlls/Lunar Compress.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/Lunar Compress.dll"
)
