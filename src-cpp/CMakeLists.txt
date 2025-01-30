cmake_minimum_required(VERSION 3.15)
project(
  CustomBuilds
  VERSION 0.1.0
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../src/")

# Detect WebAssembly (Emscripten)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    message(STATUS "Targeting WebAssembly (Emscripten)")

    # Set output file format
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    # print executable name
    message(STATUS "Executable name: ${CMAKE_PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

    # WebAssembly-Specific Flags
    set(EMSCRIPTEN_COMPILE_FLAGS
        "-s USE_WEBGL2=1 -s FULL_ES3=1 -lembind -s USE_GLFW=3 --emit-tsd noise.d.ts -s EXPORT_NAME=Wasm -s ALLOW_MEMORY_GROWTH=1  -s SINGLE_FILE=1 -s NO_DISABLE_EXCEPTION_CATCHING -s EXPORT_ES6=1 -s NO_EXIT_RUNTIME=1 -s \"EXPORTED_RUNTIME_METHODS=['stringToNewUTF8', 'UTF8ToString']\" -s EXPORT_ALL=1 -s \"EXPORTED_FUNCTIONS=['_free', '_malloc', '_main']\""
    )

    # Modify compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_COMPILE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_COMPILE_FLAGS}")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE ROOT_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Set default mode to "all" if not specified
set(PROGRAMS "all" CACHE STRING "Programs to build")

if(NOT DEFINED PROGRAMS)
  message(
    FATAL_ERROR
      "Please specify PROGRAMS to build using -DPROGRAMS=<name1;name2>, or use -DPROGRAMS=all."
  )
endif()

file(
  GLOB PROGRAM_DIRS
  RELATIVE ${CMAKE_SOURCE_DIR}/programs
  ${CMAKE_SOURCE_DIR}/programs/*)

list(REMOVE_ITEM PROGRAM_DIRS CMakeLists.txt) # Remove CMakeLists.txt if present
string(REPLACE ";" ", " VALID_PROGRAMS "${PROGRAM_DIRS}")

if("all" IN_LIST PROGRAMS)
  message(STATUS "Building all programs...")
  set(SELECTED_PROGRAMS ${PROGRAM_DIRS})
else()
  foreach(PROGRAM ${PROGRAMS})
    if(NOT PROGRAM IN_LIST PROGRAM_DIRS)
      message(FATAL_ERROR "Invalid program name '${PROGRAM}'. Valid options are: ${VALID_PROGRAMS}, or 'all'.")
    endif()
    list(APPEND SELECTED_PROGRAMS ${PROGRAM})
  endforeach()
endif()

# External Libraries Setup
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLAD_DIR "${LIB_DIR}/glad")
set(IMGUI_DIR "${LIB_DIR}/imgui")
set(FMT_DIR "${LIB_DIR}/fmt")

# Find dependencies (Threads are disabled in WebAssembly)
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

# GLFW (Only include if not WebAssembly)
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_subdirectory("${GLFW_DIR}")
endif()

# Glad
# add_library(glad "${GLAD_DIR}/src/glad.c")
# target_include_directories(glad PRIVATE "${GLAD_DIR}/include")

# ImGUI
add_library(
  "imgui"
  "${IMGUI_DIR}/imgui.cpp"
  "${IMGUI_DIR}/imgui_demo.cpp"
  "${IMGUI_DIR}/imgui_draw.cpp"
  "${IMGUI_DIR}/imgui_tables.cpp"
  "${IMGUI_DIR}/imgui_widgets.cpp"
  "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
  "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp")

target_include_directories("imgui" PRIVATE "${GLFW_DIR}/include")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}/backends")

# FMT
add_subdirectory("${FMT_DIR}")

# Build selected programs
foreach(PROG ${SELECTED_PROGRAMS})
  message(STATUS "Building program '${PROG}'...")

  file(GLOB PROGRAM_SOURCES "${CMAKE_SOURCE_DIR}/programs/${PROG}/*.cpp")
  add_executable(${PROG} ${PROGRAM_SOURCES} ${ROOT_SRC})

  target_compile_definitions(${PROG} PUBLIC ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/programs/${PROG}")

  target_include_directories(${PROG} PRIVATE "${CMAKE_SOURCE_DIR}/include/")

  # Link External Libraries
  if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
      # Threads
      target_link_libraries(${PROG} Threads::Threads)

      # GLFW
      target_link_libraries(${PROG} glfw "${GLFW_LIBRARIES}")
      target_include_directories(${PROG} PRIVATE "${GLFW_DIR}/include")
      target_compile_definitions(${PROG} PRIVATE GLFW_INCLUDE_NONE)
  else()
      # WebAssembly-specific OpenGL linking
      target_link_libraries(${PROG} "-s USE_WEBGL2=1 -s FETCH=1")
      # Include emscripten headers from emsdk installation in $EMSDK variable
      target_include_directories(${PROG} PRIVATE "$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include")
  endif()

  # Glad
  # target_include_directories(${PROG} PRIVATE "${GLAD_DIR}/include")
  # target_link_libraries(${PROG} glad ${CMAKE_DL_LIBS})

  # ImGUI
  target_include_directories(${PROG} PRIVATE "${IMGUI_DIR}")
  target_include_directories(${PROG} PRIVATE "${IMGUI_DIR}/backends")
  target_link_libraries(${PROG} "imgui" "${CMAKE_DL_LIBS}")

  # FMT
  target_link_libraries(${PROG} fmt::fmt)
  target_include_directories(${PROG} PRIVATE "${FMT_DIR}/include")
endforeach()
