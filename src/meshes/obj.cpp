#include <fstream>
#include <iostream>
#include <unordered_map>

#include <minpt/core/timer.h>
#include <minpt/meshes/obj.h>

namespace minpt {

struct Vertex {
  Vertex() noexcept = default;

  Vertex(const std::string& string) noexcept : n((std::uint32_t)-1), uv((std::uint32_t)-1) {
    auto tokens = tokenize(string, "/", true);
    p = toUInt(tokens[0]);
    if (tokens.size() >= 2 && !tokens[1].empty())
      uv = toUInt(tokens[1]);
    if (tokens.size() == 3 && !tokens[2].empty())
      n = toUInt(tokens[2]);
  }

  inline bool operator==(const Vertex& v) const {
    return p == v.p && n == v.n && uv == v.uv;
  }

  inline bool operator!=(const Vertex& v) const {
    return p != v.p && n != v.n && uv != v.uv;
  }

  std::uint32_t p, n, uv;
};

struct VertexHash : public std::unary_function<Vertex, std::size_t> {
  std::size_t operator()(const Vertex& v) const {
    auto hash = std::hash<std::uint32_t>()(v.p);
    hash = hash * 37 + std::hash<std::uint32_t>()(v.uv);
    return hash * 37 + std::hash<std::uint32_t>()(v.n);
  }
};

WavefrontOBJ::WavefrontOBJ(const PropertyList& props) {
  reverseOrientation = props.getBoolean("reverseOrientation", false);
  auto mat = props.getTransform("toWorld", Matrix4f::identity());
  transformSwapsHandedness = mat.swapsHandedness();

  name = props.getString("filename");
  auto path = getFileResolver()->resolve(name);
  std::ifstream file(path.str());
  if (file.fail())
    throw Exception("Unable to open OBJ file \"%s\"!", name);

  std::cout << "Loading \"" << name << "\" .. ";
  Timer timer;

  std::vector<Vector3f> positions;
  std::vector<Vector3f> normals;
  std::vector<Vector2f> uvs;
  std::vector<std::uint32_t> indices;
  std::vector<Vertex> vertices;
  std::unordered_map<Vertex, uint32_t, VertexHash> vertexMap;

  std::string lineStr;
  while (std::getline(file, lineStr)) {
    std::string prefix;
    std::istringstream line(lineStr);
    line >> prefix;

    if (prefix == "v") {
      Vector3f p;
      line >> p.x >> p.y >> p.z;
      p = mat.applyP(p);
      bounds.merge(p);
      positions.push_back(p);
    } else if (prefix == "vt") {
      Vector2f uv;
      line >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (prefix == "vn") {
      Vector3f n;
      line >> n.x >> n.y >> n.z;
      normals.push_back(normalize(mat.applyN(n)));
    } else if (prefix == "f") {
      Vertex verts[6];
      auto nVerts = 3;
      std::string v1, v2, v3, v4;
      line >> v1 >> v2 >> v3 >> v4;

      verts[0] = Vertex(v1);
      verts[1] = Vertex(v2);
      verts[2] = Vertex(v3);

      if (!v4.empty()) {
        verts[3] = Vertex(v4);
        verts[4] = verts[0];
        verts[5] = verts[2];
        nVerts = 6;
      }

      for (auto i = 0; i < nVerts; ++i) {
        auto& v = verts[i];
        auto it = vertexMap.find(v);
        if (it == vertexMap.end()) {
          vertexMap[v] = (uint32_t)vertices.size();
          indices.push_back((uint32_t)vertices.size());
          vertices.push_back(v);
        } else {
          indices.push_back(it->second);
        }
      }
    }
  }

  nVertices = (std::uint32_t)vertices.size();
  nTriangles = (std::uint32_t)(indices.size() / 3);

  f.reset(new std::uint32_t[indices.size()]);
  memcpy(f.get(), indices.data(), sizeof(std::uint32_t) * indices.size());

  p.reset(new Vector3f[nVertices]);
  for (auto i = 0u; i < nVertices; ++i)
    p[i] = positions[vertices[i].p - 1];

  if (!normals.empty()) {
    n.reset(new Vector3f[nVertices]);
    for (auto i = 0u; i < nVertices; ++i)
      n[i] = normals[vertices[i].n - 1];
  }

  if (!uvs.empty()) {
    uv.reset(new Vector2f[nVertices]);
    for (auto i = 0u; i < nVertices; ++i)
      uv[i] = uvs[vertices[i].uv - 1];
  }

  std::cout
    << "done. (V=" << nVertices << ", F=" << nTriangles << ", took "
    << timer.elapsedString() << " and "
    << memString(
      nTriangles * 3 * sizeof(uint32_t) +
      nVertices * sizeof(Vector3f) +
      (n ? nVertices * sizeof(Vector3f) : 0) +
      (uv ? nVertices * sizeof(Vector2f) : 0))
    << ")" << std::endl;
}

}
