#include <tbb/parallel_for.h>
#include <minpt/utils/bitmap.h>
#include <minpt/core/timer.h>
#include <minpt/core/scene.h>
#include <minpt/lights/area.h>

namespace minpt {

void Scene::addChild(Object* child) {
  switch (child->getClassType()) {
    case EMesh: {
        if (!accel)
          throw Exception("Set accelerator first before add any mesh!");
        auto mesh = static_cast<Mesh*>(child);
        accel->addMesh(mesh);
        meshes.push_back(mesh);
        if (mesh->light)
          lights.push_back(mesh->light);
      }
      break;
    case ESampler:
      if (sampler)
        throw Exception("There can only be one sampler per scene!");
      sampler = static_cast<Sampler*>(child);
      break;
    case ECamera:
      if (camera)
        throw Exception("There can only be one camera per scene!");
      camera = static_cast<Camera*>(child);
      break;
    case EIntegrator:
      if (integrator)
        throw Exception("There can only be one integrator per scene!");
      integrator = static_cast<Integrator*>(child);
      break;
    case EAccel:
      if (accel)
        throw Exception("There can only be one accelerator per scene!");
      accel = static_cast<Accelerator*>(child);
      break;
    case ELight:
      lights.push_back(static_cast<Light*>(child));
      break;
    default:
      throw Exception("Scene::addChild(<%s>) is not supported!", classTypeName(child->getClassType()));
  }
}

std::string Scene::toString() const {
  std::string meshStr;
  if (meshes.empty())
    meshStr = "{}";
  else {
    for (std::size_t i = 0, length = meshes.size(); i < length; ++i) {
      meshStr += std::string("  ") + indent(meshes[i]->toString());
      if (i + 1 < length)
        meshStr += ",";
      meshStr += "\n";
    }
    meshStr = tfm::format(
      "{\n%s}",
      meshStr
    );
  }

  std::string lightStr;
  if (lights.empty())
    lightStr = "{}";
  else {
    for (std::size_t i = 0, length = lights.size(); i < length; ++i) {
      lightStr += std::string("  ") + indent(lights[i]->toString());
      if (i + 1 < length)
        lightStr += ",";
      lightStr += "\n";
    }
    lightStr = tfm::format(
      "{\n%s}",
      lightStr
    );
  }

  return tfm::format(
    "Scene[\n"
    "  integrator = %s,\n"
    "  accelerator = %s,\n"
    "  sampler = %s,\n"
    "  camera = %s,\n"
    "  meshes = %s,\n"
    "  lights = %s,\n"
    "  outputName=\"%s\"\n"
    "]",
    indent(integrator->toString()),
    indent(accel->toString()),
    indent(sampler->toString()),
    indent(camera->toString()),
    indent(meshStr),
    indent(lightStr),
    indent(outputName)
  );
}

}
