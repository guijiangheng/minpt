#include <fstream>
#include <unordered_map>
#include <minpt/utils/utils.h>
#include <minpt/utils/obj.h>

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

WavefrontOBJ::WavefrontOBJ(const std::string& filename) {
  auto path = getFileResolver()->resolve(filename);
  std::ifstream file(path.str());
  if (file.fail())
    throw Exception("Unable to open OBJ file \"%s\"!", filename);

  name = filename;

  std::vector<Vector3f> positions;
  std::vector<Vector3f> normals;
  std::vector<Vector2f> uvs;
  std::vector<int> indices;
  std::vector<Vertex> vertices;
  std::unordered_map<Vertex, uint32_t, VertexHash> vertexMap;

  std::string lineStr;
  while (std::getline(file, lineStr)) {
    std::string prefix;
    std::istringstream line(lineStr);
    line >> prefix;

    if (prefix == "v") {
      Vector3f p;
      line >> p.x() >> p.y() >> p.z();
      positions.push_back(p);
    } else if (prefix == "vt") {
      Vector2f uv;
      line >> uv.x() >> uv.y();
      uvs.push_back(uv);
    } else if (prefix == "vn") {
      Vector3f n;
      line >> n.x() >> n.y() >> n.z();
      normals.push_back(n.normalized());
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

  auto nVerts = (int)vertices.size();

  f.resize(3, indices.size() / 3);
  memcpy(f.data(), indices.data(), sizeof(uint32_t) * indices.size());

  v.resize(3, nVerts);
  for (auto i = 0; i < nVerts; ++i)
    v.col(i) = positions[vertices[i].p - 1];

  if (!normals.empty()) {
    n.resize(3, nVerts);
    for (auto i = 0; i < nVerts; ++i)
      n.col(i) = normals[vertices[i].n - 1];
  }

  if (!uvs.empty()) {
    uv.resize(2, nVerts);
    for (auto i = 0; i < nVerts; ++i)
      uv.col(i) = uvs[vertices[i].uv - 1];
  }
}

}
