#include <minpt/math/matrix4.h>
#include <minpt/core/exception.h>

namespace minpt {

Matrix4f& Matrix4f::inverse() {
  int idxr[4], idxc[4];
  bool visited[4] = { false, false, false, false };

  for (auto i = 0; i < 4; ++i) {
    auto row = 0;
    auto col = 0;
    auto pivot = 0.0f;
    for (auto r = 0; r < 4; ++r) {
      if (visited[r]) continue;
      for (auto c = 0; c < 4; ++c) {
        auto abs = std::abs(e[r][c]);
        if (visited[c] || abs <= pivot) continue;
        pivot = abs;
        row = r;
        col = c;
      }
    }

    if (pivot == 0) throw Exception("Singular matrix!");

    visited[col] = true;
    if (row != col)
      for (auto c = 0; c < 4; ++c) std::swap(e[row][c], e[col][c]);

    idxr[i] = row;
    idxc[i] = col;
    auto inv = 1 / e[col][col];
    e[col][col] = 1;

    for (auto c = 0; c < 4; ++c)
      e[col][c] *= inv;

    for (auto r = 0; r < 4; ++r) {
      if (r == col) continue;
      auto tmp = e[r][col];
      e[r][col] = 0;
      for (auto c = 0; c < 4; ++c)
        e[r][c] -= e[col][c] * tmp;
    }
  }

  for (auto i = 2; i >= 0; --i) {
    if (idxr[i] == idxc[i]) continue;
    for (auto r = 0; r < 4; ++r)
      std::swap(e[r][idxr[i]], e[r][idxc[i]]);
  }

  return *this;
}

Matrix4f& Matrix4f::transpose() {
  float tmp;
  tmp = e[0][1]; e[0][1] = e[1][0]; e[1][0] = tmp;
  tmp = e[0][2]; e[0][2] = e[2][0]; e[2][0] = tmp;
  tmp = e[0][3]; e[0][3] = e[3][0]; e[3][0] = tmp;
  tmp = e[1][2]; e[1][2] = e[2][1]; e[2][1] = tmp;
  tmp = e[1][3]; e[1][3] = e[3][1]; e[3][1] = tmp;
  tmp = e[2][3]; e[2][3] = e[3][2]; e[3][2] = tmp;
  return *this;
}

}
