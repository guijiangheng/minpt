#include <minpt/accels/bvh.h>
#include <minpt/cameras/perspective.h>

#include <minpt/bsdfs/diffuse.h>
#include <minpt/bsdfs/glass.h>
#include <minpt/bsdfs/mirror.h>
#include <minpt/bsdfs/plastic.h>

#include <minpt/filters/box.h>
#include <minpt/filters/gaussian.h>

#include <minpt/integrators/ao.h>
#include <minpt/integrators/normals.h>
#include <minpt/integrators/direct.h>
#include <minpt/integrators/path.h>

#include <minpt/lights/area.h>
#include <minpt/lights/point.h>

#include <minpt/meshes/obj.h>
#include <minpt/samplers/random.h>

namespace minpt {

std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

MINPT_REGISTER_CLASS(Scene, "scene");
MINPT_REGISTER_CLASS(RandomSampler, "random");
MINPT_REGISTER_CLASS(BVHAccel, "bvh");
MINPT_REGISTER_CLASS(PerspectiveCamera, "perspective");
MINPT_REGISTER_CLASS(WavefrontOBJ, "obj");

MINPT_REGISTER_CLASS(Diffuse, "diffuse");
MINPT_REGISTER_CLASS(Glass, "glass");
MINPT_REGISTER_CLASS(Mirror, "mirror");
MINPT_REGISTER_CLASS(Plastic, "plastic");

MINPT_REGISTER_CLASS(BoxFilter, "box");
MINPT_REGISTER_CLASS(GaussianFilter, "gaussian");

MINPT_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");
MINPT_REGISTER_CLASS(NormalIntegrator, "normals");
MINPT_REGISTER_CLASS(DirectIntegrator, "direct");
MINPT_REGISTER_CLASS(PathIntegrator, "path");

MINPT_REGISTER_CLASS(PointLight, "point");
MINPT_REGISTER_CLASS(AreaLight, "area");

}
