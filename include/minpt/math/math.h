#pragma once

#include <Eigen/Core>
#include <minpt/math/bounds.h>
#include <minpt/math/color.h>
#include <minpt/math/vector.h>

namespace minpt {

using MatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
using MatrixXu = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

}
