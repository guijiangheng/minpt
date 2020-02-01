#pragma once

#include <string>
#include <vector>
#include <filesystem/resolver.h>

#include <minpt/core/exception.h>
#include <minpt/math/math.h>
#include <minpt/core/spectrum.h>

namespace minpt {

bool toBool(const std::string& string);

int toInt(const std::string& string);

unsigned int toUInt(const std::string& string);

float toFloat(const std::string& string);

Vector2i toVector2i(const std::string& string);

Vector2f toVector2f(const std::string& string);

Vector3f toVector3f(const std::string& string);

RGBSpectrum toRGBSpectrum(const std::string& string);

std::string trim(const std::string& string);

std::string toLower(const std::string& string);

std::string indent(const std::string& string, int amount = 2);

std::vector<std::string> tokenize(const std::string& string, const std::string& delim = ", ", bool includeEmpty = false);

bool endsWith(const std::string& string, const std::string& ending);

std::string timeString(double time, bool precise = false);

std::string memString(size_t size, bool precise = false);

filesystem::resolver* getFileResolver();

}
