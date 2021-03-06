#include <minpt/accels/bvh.h>
#include <minpt/cameras/perspective.h>

#include <minpt/bsdfs/diffuse.h>
#include <minpt/bsdfs/glass.h>
#include <minpt/bsdfs/metal.h>
#include <minpt/bsdfs/mirror.h>
#include <minpt/bsdfs/plastic.h>
#include <minpt/bsdfs/roughconductor.h>
#include <minpt/bsdfs/roughdielectric.h>

#include <minpt/filters/box.h>
#include <minpt/filters/gaussian.h>

#include <minpt/integrators/ao.h>
#include <minpt/integrators/normals.h>
#include <minpt/integrators/direct.h>
#include <minpt/integrators/path.h>
#include <minpt/integrators/path_simple.h>

#include <minpt/lights/area.h>
#include <minpt/lights/point.h>
#include <minpt/lights/constant.h>
#include <minpt/lights/envlight.h>

#include <minpt/meshes/obj.h>
#include <minpt/meshes/ply.h>
#include <minpt/samplers/random.h>

#include <minpt/textures/constant.h>
#include <minpt/textures/checkerboard.h>
#include <minpt/textures/image.h>

namespace minpt {

std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

MINPT_REGISTER_CLASS(Scene, "scene");
MINPT_REGISTER_CLASS(RandomSampler, "random");
MINPT_REGISTER_CLASS(BVHAccel, "bvh");
MINPT_REGISTER_CLASS(PerspectiveCamera, "perspective");

MINPT_REGISTER_CLASS(PLY, "ply");
MINPT_REGISTER_CLASS(WavefrontOBJ, "obj");

MINPT_REGISTER_CLASS(Diffuse, "diffuse");
MINPT_REGISTER_CLASS(Glass, "glass");
MINPT_REGISTER_CLASS(Metal, "metal");
MINPT_REGISTER_CLASS(Mirror, "mirror");
MINPT_REGISTER_CLASS(Plastic, "plastic");
MINPT_REGISTER_CLASS(RoughConductor, "roughconductor");
MINPT_REGISTER_CLASS(RoughDielectric, "roughdielectric");

MINPT_REGISTER_CLASS(BoxFilter, "box");
MINPT_REGISTER_CLASS(GaussianFilter, "gaussian");

MINPT_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");
MINPT_REGISTER_CLASS(NormalIntegrator, "normals");
MINPT_REGISTER_CLASS(DirectIntegrator, "direct");
MINPT_REGISTER_CLASS(PathIntegrator, "path");
MINPT_REGISTER_CLASS(PathSimpleIntegrator, "path_simple");

MINPT_REGISTER_CLASS(PointLight, "point");
MINPT_REGISTER_CLASS(AreaLight, "area");
MINPT_REGISTER_CLASS(ConstantEnvLight, "constant_env");
MINPT_REGISTER_CLASS(EnvironmentLight, "envmap");

MINPT_REGISTER_TEMPLATED_CLASS(ConstantTexture, float, "constant_float");
MINPT_REGISTER_TEMPLATED_CLASS(ConstantTexture, RGBSpectrum, "constant_color");
MINPT_REGISTER_TEMPLATED_CLASS(CheckerboardTexture, float, "checkerboard_float");
MINPT_REGISTER_TEMPLATED_CLASS(CheckerboardTexture, RGBSpectrum, "checkerboard_color");
MINPT_REGISTER_TEMPLATED_CLASS(ImageTexture, float, "image_float");
MINPT_REGISTER_TEMPLATED_CLASS(ImageTexture, RGBSpectrum, "image_color");

}
