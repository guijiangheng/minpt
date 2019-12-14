#pragma once

#include <memory>
#include <minpt/core/sampling.h>
#include <minpt/core/interaction.h>
#include <minpt/core/distribution.h>

namespace minpt {

class AreaLight;

struct LightSample {
  Vector3f p;
  Vector3f n;
};

/**
 * \brief Triangle mesh
 *
 * This class stores a triangle mesh object and provides numerous functions
 * for querying the individual triangles. Subclasses of \c Mesh implement
 * the specifics of how to create its contents (e.g. by loading from an
 * external file)
 */
class Mesh : public Object {
public:
  virtual ~Mesh();

  void addChild(Object* object) override;

  void activate() override {
    auto nPrims = getPrimitiveCount();
    pdf.reserve(nPrims);
    for (std::uint32_t i = 0; i < nPrims; ++i)
      pdf.append(getSurfaceArea(i));
    pdf.normalize();

    totalArea = pdf.getSum();
    totalAreaInv = 1 / totalArea;

    if (!light && !bsdf)
      bsdf = static_cast<BSDF*>(ObjectFactory::createInstance("diffuse", PropertyList()));
  }

  std::uint32_t getPrimitiveCount() const {
    return nTriangles;
  }

  float getTotalArea() const {
    return totalArea;
  }

  float getSurfaceArea(std::uint32_t index) const {
    auto offset = 3 * index;
    auto& a = p[f[offset]];
    auto& b = p[f[offset + 1]];
    auto& c = p[f[offset + 2]];
    return cross(b - a, b - c).length() / 2;
  }

  Bounds3f getBounds(int index) const {
    auto offset = 3 * index;
    auto& a = p[f[offset]];
    auto& b = p[f[offset + 1]];
    auto& c = p[f[offset + 2]];
    return merge(Bounds3f(min(a, b), max(a, b)), c);
  }

  LightSample sample(Vector2f& u, float& _pdf) const {
    _pdf = totalAreaInv;
    auto index = pdf.sampleReuse(u.x);
    auto uv = uniformSampleTriangle(u);
    auto& a = p[f[3 * index]];
    auto& b = p[f[3 * index + 1]];
    auto& c = p[f[3 * index + 2]];
    return { barycentric(a, b, c, uv), normalize(cross(b - a, c - a)) };
  }

  LightSample sample(const Vector3f& ref, Vector2f& u, float& _pdf) const {
    auto pLight = sample(u, _pdf);
    auto d = pLight.p - ref;
    auto w = normalize(d);
    _pdf *= d.lengthSquared() / absdot(pLight.n, w);
    return pLight;
  }

  bool intersect(std::uint32_t index, const Ray& ray) const;

  bool intersect(std::uint32_t index, const Ray& ray, Interaction& isect) const;

  void computeIntersection(std::uint32_t index, Interaction& isect) const;

  EClassType getClassType() const override {
    return EMesh;
  }

  std::string toString() const override;

protected:
  Mesh() = default;

protected:
  float totalArea;
  float totalAreaInv;
  std::uint32_t nVertices;
  std::uint32_t nTriangles;
  std::unique_ptr<Vector3f[]> p;
  std::unique_ptr<Vector3f[]> n;
  std::unique_ptr<Vector2f[]> uv;
  std::unique_ptr<std::uint32_t[]> f;
  Distribution1D pdf;

public:
  std::string name;
  Bounds3f bounds;
  BSDF* bsdf = nullptr;
  AreaLight* light = nullptr;
};

}
