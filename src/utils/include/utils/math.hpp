#ifndef MATH_UTILS
#define MATH_UTILS

namespace math {
constexpr double PI = 3.1415926535897932384626433832795;
constexpr double TAU = 2.*PI;
constexpr double PI2 = PI/2.;
constexpr double PI4 = PI/4.;
constexpr double PI8 = PI/8.;

constexpr float PI_F = static_cast<float>(PI);
constexpr float TAU_F = static_cast<float>(TAU);
constexpr float PI2_F = static_cast<float>(PI2);
constexpr float PI4_F = static_cast<float>(PI4);
constexpr float PI8_F = static_cast<float>(PI8);

constexpr double GOLDEN_RATIO = 1.6180339887498948482045868343656381177203091798057628621354486227;
constexpr float GOLDEN_RATIO_F = static_cast<float>(GOLDEN_RATIO);
}

#endif  // !_SIMULATEDUNIT_
