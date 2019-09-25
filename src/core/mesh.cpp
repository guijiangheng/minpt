#include <minpt/core/mesh.h>

namespace minpt {

bool Mesh::intersect(uint32_t index, const Ray3f& ray) const {
  auto& a = v.col(f(0, index));
  auto& b = v.col(f(1, index));
  auto& c = v.col(f(2, index));

  Vector3f e1 = b - a;
  Vector3f e2 = c - a;
  Vector3f p = ray.d.cross(e2);
  auto det = p.dot(e1);
  if (std::abs(det) < 0.000001f) return false;

  Vector3f t = ray.o - a;
  auto detInv = 1 / det;
  auto u = p.dot(t) * detInv;
  if (u < 0 || u > 1) return false;

  Vector3f q = t.cross(e1);
  auto v = q.dot(ray.d) * detInv;
  if (v < 0 || u + v > 1) return false;

  auto dist = q.dot(e2) * detInv;
  if (dist <= 0 || dist > ray.tMax) return false;

  return true;
}

// ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Mesh::intersect(uint32_t index, const Ray3f& ray, Interaction& isect) const {
  auto& a = v.col(f(0, index));
  auto& b = v.col(f(1, index));
  auto& c = v.col(f(2, index));

  Vector3f e1 = b - a;
  Vector3f e2 = c - a;
  Vector3f p = ray.d.cross(e2);
  auto det = p.dot(e1);
  if (std::abs(det) < 0.000001f) return false;

  Vector3f t = ray.o - a;
  auto detInv = 1 / det;
  auto u = p.dot(t) * detInv;
  if (u < 0 || u > 1) return false;

  Vector3f q = t.cross(e1);
  auto v = q.dot(ray.d) * detInv;
  if (v < 0 || u + v > 1) return false;

  auto dist = q.dot(e2) * detInv;
  if (dist <= 0 || dist > ray.tMax) return false;

  ray.tMax = dist;
  isect.uv = Vector2f(u, v);

  return true;
}

}
