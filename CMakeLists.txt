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
  # Lodepng library for image read write
  ${LODEPNG_INCLUDE_DIR}
)

add_definitions(${NANOGUI_EXTRA_DEFS})

add_executable(minpt
  include/minpt/utils/utils.h
  include/minpt/utils/bitmap.h

  include/minpt/core/exception.h
  include/minpt/core/proplist.h
  include/minpt/core/object.h
  include/minpt/core/parser.h
  include/minpt/core/timer.h
  include/minpt/core/ray.h
  include/minpt/core/bsdf.h
  include/minpt/core/interaction.h
  include/minpt/core/mesh.h
  include/minpt/core/sampler.h
  include/minpt/core/sampling.h
  include/minpt/core/camera.h
  include/minpt/core/scene.h
  include/minpt/core/integrator.h
  include/minpt/core/accelerator.h

  include/minpt/math/math.h
  include/minpt/math/bounds2.h
  include/minpt/math/bounds3.h
  include/minpt/math/color3.h
  include/minpt/math/vector2.h
  include/minpt/math/vector3.h
  include/minpt/math/matrix4.h
  include/minpt/math/frame.h

  src/main/main.cpp
  src/math/matrix4.cpp
  src/utils/utils.cpp
  src/utils/bitmap.cpp
  src/core/scene.cpp
  src/core/mesh.cpp
  src/core/parser.cpp
  src/core/object.cpp
  src/bsdfs/diffuse.cpp
  src/meshes/obj.cpp
  src/samplers/random.cpp
  src/accelerators/bvh.cpp
  src/cameras/perspective.cpp
  src/integrators/ao.cpp
  src/integrators/normals.cpp
)

target_compile_features(minpt PRIVATE cxx_std_17)
target_include_directories(minpt PRIVATE ${MINPT_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(minpt PRIVATE tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS} lodepng)

add_executable(warptest src/main/warptest.cpp)
target_compile_features(warptest PRIVATE cxx_std_17)
target_include_directories(warptest PRIVATE ${MINPT_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(warptest PRIVATE tbb_static nanogui ${NANOGUI_EXTRA_LIBS})
