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

set(
  MINPT_HEADERS
  include/minpt/utils/utils.h
  include/minpt/utils/bitmap.h
  include/minpt/utils/exception.h

  include/minpt/core/ray.h
  include/minpt/core/interaction.h
  include/minpt/core/mesh.h
  include/minpt/core/sampler.h

  include/minpt/samplers/random.h

  include/minpt/math/math.h
  include/minpt/math/bounds.h
  include/minpt/math/color.h
  include/minpt/math/vector.h
  include/minpt/math/frame.h
)

set(
  MINPT_SRCS
  src/utils/utils.cpp
  src/utils/bitmap.cpp
  src/core/mesh.cpp
  src/utils/obj.cpp
)

add_library(minpt STATIC ${MINPT_HEADERS} ${MINPT_SRCS})
target_compile_features(minpt PUBLIC cxx_std_17)
target_include_directories(minpt PUBLIC ${MINPT_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(minpt PUBLIC tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS})

add_executable(main src/main/main.cpp)
target_link_libraries(main PRIVATE minpt)
