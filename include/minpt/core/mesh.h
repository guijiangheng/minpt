#pragma once

#include <memory>
#include <minpt/core/bsdf.h>
#include <minpt/core/sampling.h>
#include <minpt/core/interaction.h>
#include <minpt/core/distribution.h>

namespace minpt {

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
  virtual ~Mesh() {
    delete bsdf;
  };

  void addChild(Object* object) override {
    switch (object->getClassType()) {
      case EBSDF:
        if (bsdf)
          throw Exception("Mesh: tried to register multiple BSDF instances!");
        bsdf = static_cast<BSDF*>(object);
        break;
      default:
        throw Exception("Shape::addChild<%s> is not supported!", classTypeName(object->getClassType()));
    }
  }

  void activate() override {
    auto nPrims = getPrimitiveCount();
    pdf.reserve(nPrims);
    for (std::uint32_t i = 0; i < nPrims; ++i)
      pdf.append(getSurfaceArea(i));
    pdf.normalize();
  }

  std::uint32_t getPrimitiveCount() const {
    return nTriangles;
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

  Interaction sample(Vector2f& u, float& _pdf) const {
    _pdf = 1.0f / pdf.sum;

    auto index = pdf.sampleReuse(u.x);
    auto uv = uniformSampleTriangle(u);

    auto offset = 3 * index;
    auto& a = p[f[offset]];
    auto& b = p[f[offset + 1]];
    auto& c = p[f[offset + 2]];

    Interaction isect;
    isect.p = barycentric(a, b, c, uv);
    isect.n = normalize(cross(b - a, c - a));

    return isect;
  }

  bool intersect(std::uint32_t index, const Ray& ray) const;

  bool intersect(std::uint32_t index, const Ray& ray, Interaction& isect) const;

  void computeIntersection(std::uint32_t index, Interaction& isect) const;

  EClassType getClassType() const override {
    return EMesh;
  }

  std::string toString() const override {
    return tfm::format(
      "Mesh[\n"
      "  name = \"%s\",\n"
      "  vertexCount = %i,\n"
      "  triangleCount = %i,\n"
      "  bsdf = %s\n"
      "]",
      name, nVertices, nTriangles,
      bsdf ? indent(bsdf->toString()) : std::string("null")
    );
  }

protected:
  Mesh() = default;

public:
  std::string name;
  std::uint32_t nVertices;
  std::uint32_t nTriangles;
  std::unique_ptr<Vector3f[]> p;
  std::unique_ptr<Vector3f[]> n;
  std::unique_ptr<Vector2f[]> uv;
  std::unique_ptr<std::uint32_t[]> f;
  Bounds3f bounds;
  Distribution1D pdf;
  BSDF* bsdf = nullptr;
};

}
