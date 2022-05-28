#pragma once
#include "vec.h"
#include "constants.h"

constexpr math::vec<int, 2> content_dims{1920, 1080};
constexpr math::vec<int, 2> usable_area{1920, 1088};
constexpr math::vec<int, 2> aspect_ratio{content_dims / math::gcd(content_dims[0], content_dims[1])};
constexpr math::dvec2 cube_size = {64.0, 64.0};
constexpr math::dvec2 spike_size = {cube_size[0], cube_size[1] / 2};