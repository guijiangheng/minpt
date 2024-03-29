cmake_minimum_required(VERSION 3.2)
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

set(MINPT_HEADERS
  include/minpt/utils/utils.h
  include/minpt/utils/bitmap.h

  include/minpt/core/exception.h
  include/minpt/core/proplist.h
  include/minpt/core/object.h
  include/minpt/core/parser.h
  include/minpt/core/block.h
  include/minpt/core/distribution.h
  include/minpt/core/timer.h
  include/minpt/core/ray.h
  include/minpt/core/bsdf.h
  include/minpt/core/interaction.h
  include/minpt/core/mesh.h
  include/minpt/core/spectrum.h
  include/minpt/core/sampler.h
  include/minpt/core/sampling.h
  include/minpt/core/camera.h
  include/minpt/core/light.h
  include/minpt/core/filter.h
  include/minpt/core/mipmap.h
  include/minpt/core/texture.h
  include/minpt/core/scene.h
  include/minpt/core/integrator.h
  include/minpt/core/accelerator.h
  include/minpt/core/visibilitytester.h

  include/minpt/math/math.h
  include/minpt/math/common.h
  include/minpt/math/bounds2.h
  include/minpt/math/bounds3.h
  include/minpt/math/vector2.h
  include/minpt/math/vector3.h
  include/minpt/math/matrix4.h
  include/minpt/math/frame.h

  include/minpt/accels/bvh.h

  include/minpt/bsdfs/metal.h
  include/minpt/bsdfs/glass.h
  include/minpt/bsdfs/mirror.h
  include/minpt/bsdfs/diffuse.h
  include/minpt/bsdfs/plastic.h
  include/minpt/bsdfs/roughconductor.h
  include/minpt/bsdfs/roughdielectric.h

  include/minpt/cameras/perspective.h

  include/minpt/filters/box.h
  include/minpt/filters/gaussian.h

  include/minpt/gui/screen.h

  include/minpt/integrators/ao.h
  include/minpt/integrators/normals.h
  include/minpt/integrators/direct.h
  include/minpt/integrators/path.h
  include/minpt/integrators/path_simple.h

  include/minpt/lights/point.h
  include/minpt/lights/area.h
  include/minpt/lights/constant.h
  include/minpt/lights/envlight.h

  include/minpt/meshes/obj.h
  include/minpt/meshes/ply.h

  include/minpt/microfacets/beckmann.h
  include/minpt/microfacets/trowbridge.h

  include/minpt/samplers/random.h

  include/minpt/textures/constant.h
  include/minpt/textures/checkerboard.h
  include/minpt/textures/image.h
)

set(MINPT_SRCS
  src/math/vector3.cpp
  src/math/matrix4.cpp

  src/utils/utils.cpp
  src/utils/bitmap.cpp

  src/core/bsdf.cpp
  src/core/scene.cpp
  src/core/spectrum.cpp
  src/core/mesh.cpp
  src/core/parser.cpp
  src/core/object.cpp
  src/core/block.cpp
  src/core/interaction.cpp
  src/core/visibilitytester.cpp

  src/gui/screen.cpp

  src/microfacets/beckmann.cpp
  src/microfacets/trowbridge.cpp

  src/meshes/obj.cpp
  src/meshes/ply.cpp

  src/accels/bvh.cpp

  src/cameras/perspective.cpp

  src/integrators/path.cpp
  src/integrators/direct.cpp

  src/bsdfs/glass.cpp
  src/bsdfs/plastic.cpp
  src/bsdfs/roughconductor.cpp
  src/bsdfs/roughdielectric.cpp
)

add_library(minpt STATIC ${MINPT_HEADERS} ${MINPT_SRCS})
target_compile_features(minpt PUBLIC cxx_std_20)
target_include_directories(minpt PUBLIC ${MINPT_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(minpt PUBLIC NOMINMAX)

if(WIN32)
  target_link_libraries(minpt PUBLIC tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS} lodepng zlibstatic)
  else()
  target_link_libraries(minpt PUBLIC tbb_static pugixml IlmImf nanogui ${NANOGUI_EXTRA_LIBS} lodepng)
endif()

add_executable(main src/main/main.cpp)
target_link_libraries(main minpt)
set_target_properties(main PROPERTIES OUTPUT_NAME minpt)

add_executable(warptest src/main/warptest.cpp)
target_link_libraries(warptest minpt)
target_compile_definitions(warptest PUBLIC NOMINMAX)
