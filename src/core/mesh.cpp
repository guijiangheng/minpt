#include <minpt/math/math.h>
#include <minpt/core/mesh.h>

namespace minpt {

bool Mesh::intersect(uint32_t index, const Ray3f& ray) const {
  Vector3f a = v.col(f(0, index));
  Vector3f b = v.col(f(1, index));
  Vector3f c = v.col(f(2, index));

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

void Mesh::computeIntersection(std::uint32_t index, Interaction& isect) const {
  auto ia = f(0, index);
  auto ib = f(1, index);
  auto ic = f(2, index);

  Vector3f a = v.col(f(0, index));
  Vector3f b = v.col(f(1, index));
  Vector3f c = v.col(f(2, index));
  isect.p = barycentric(a, b, c, isect.uv);
  isect.n = (b - a).cross(c - a).normalized();

  if (n.size()) {
    Vector3f na = n.col(ia);
    Vector3f nb = n.col(ib);
    Vector3f nc = n.col(ic);
    Vector3f ns = barycentric(na, nb, nc, isect.uv).normalized();
    isect.shFrame = Frame(ns);
  } else {
    isect.shFrame = Frame(isect.n);
  }

  if (uv.size()) {
    Vector2f uva = uv.col(f(0, index));
    Vector2f uvb = uv.col(f(1, index));
    Vector2f uvc = uv.col(f(2, index));
    isect.uv = barycentric(uva, uvb, uvc, isect.uv);
  }
}

}
