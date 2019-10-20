#include <minpt/math/math.h>
#include <minpt/core/mesh.h>

namespace minpt {

bool Mesh::intersect(uint32_t index, const Ray& ray) const {
  auto& a = p[f[3 * index]];
  auto& b = p[f[3 * index + 1]];
  auto& c = p[f[3 * index + 2]];

  auto e1 = b - a;
  auto e2 = c - a;
  auto p = cross(ray.d, e2);
  auto det = dot(p, e1);
  if (std::abs(det) < 0.000001f) return false;

  auto t = ray.o - a;
  auto detInv = 1 / det;
  auto u = dot(p, t) * detInv;
  if (u < 0 || u > 1) return false;

  auto q = cross(t, e1);
  auto v = dot(q, ray.d) * detInv;
  if (v < 0 || u + v > 1) return false;

  auto dist = dot(q, e2) * detInv;
  if (dist <= 0 || dist > ray.tMax) return false;

  return true;
}

// ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Mesh::intersect(uint32_t index, const Ray& ray, Interaction& isect) const {
  auto& a = p[f[3 * index]];
  auto& b = p[f[3 * index + 1]];
  auto& c = p[f[3 * index + 2]];

  auto e1 = b - a;
  auto e2 = c - a;
  auto p = cross(ray.d, e2);
  auto det = dot(p, e1);
  if (std::abs(det) < 0.000001f) return false;

  auto t = ray.o - a;
  auto detInv = 1 / det;
  auto u = dot(p, t) * detInv;
  if (u < 0 || u > 1) return false;

  auto q = cross(t, e1);
  auto v = dot(q, ray.d) * detInv;
  if (v < 0 || u + v > 1) return false;

  auto dist = dot(q, e2) * detInv;
  if (dist <= 0 || dist > ray.tMax) return false;

  ray.tMax = dist;
  isect.uv = Vector2f(u, v);

  return true;
}

void Mesh::computeIntersection(std::uint32_t index, Interaction& isect) const {
  auto ia = f[3 * index];
  auto ib = f[3 * index + 1];
  auto ic = f[3 * index + 2];

  auto& a = p[ia];
  auto& b = p[ib];
  auto& c = p[ic];
  isect.p = barycentric(a, b, c, isect.uv);
  isect.n = normalize(cross(c - a, b - a));

  if (n) {
    auto ns = normalize(barycentric(n[ia], n[ib], n[ic], isect.uv));
    isect.shFrame = Frame(ns);
  } else {
    isect.shFrame = Frame(isect.n);
  }

  if (uv) {
    isect.uv = barycentric(uv[ia], uv[ib], uv[ic], isect.uv);
  }
}

}
