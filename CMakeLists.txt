cmake_minimum_required(VERSION 3.0)
project(minpt LANGUAGES CXX)

add_subdirectory(ext ext_build)

set(
  MINPT_INCLUDE_DIRS
  # Nori include files
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  # tinyformat string formatting library
  ${TFM_INCLUDE_DIR}
  # Eigen linear algebra library
  ${EIGEN_INCLUDE_DIR}
  # OpenEXR high dynamic range bitmap library
  ${OPENEXR_INCLUDE_DIRS}
  # Intel Thread Building Blocks
  ${TBB_INCLUDE_DIR}
  # Pseudorandom number generator
  ${PCG32_INCLUDE_DIR}
  # PugiXML parser
  ${PUGIXML_INCLUDE_DIR}
  # Helper functions for statistical hypothesis tests
  ${HYPOTHESIS_INCLUDE_DIR}
  # GLFW library for OpenGL context creation
  ${GLFW_INCLUDE_DIR}
  # GLEW library for accessing OpenGL functions
  ${GLEW_INCLUDE_DIR}
  # NanoVG drawing library
  ${NANOVG_INCLUDE_DIR}
  # NanoGUI user interface library
  ${NANOGUI_INCLUDE_DIR}
  ${NANOGUI_EXTRA_INCS}
  # Portable filesystem API
  ${FILESYSTEM_INCLUDE_DIR}
)

add_definitions(${NANOGUI_EXTRA_DEFS})

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_include_directories(main PRIVATE ${MINPT_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(main PRIVATE tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS})
