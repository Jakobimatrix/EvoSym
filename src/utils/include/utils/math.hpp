#ifndef MATH_UTILS
#define MATH_UTILS

#include <cmath>
#include <type_traits>

namespace math {
#define M_TAU = 2. * M_PI;


#define GOLDEN_RATIO \
  = 1.6180339887498948482045868343656381177203091798057628621354486227;

template <class T>
inline T deg2Rad(T deg) {
  return deg / 360. * static_cast<T>(M_PI);
}

template <class T>
inline T rad2Deg(T rad) {
  return rad * 360. / static_cast<T>(M_PI);
}

template <typename T1, typename T2>
inline auto positiveModulo_detail(T1 x, T2 y, std::true_type, std::false_type) {
  static_assert(std::is_signed<T1>::value && std::is_signed<T2>::value,
                "you passed an unsigned value to positiveModulo. "
                "positiveModulo only works with signed integer or floating "
                "point parameters");
  // see
  // https://stackoverflow.com/questions/14997165/fastest-way-to-get-a-positive-modulo-in-c-c
  return (x % y + y) % y;
}

template <typename T1, typename T2>
inline auto positiveModulo_detail(T1 x, T2 y, std::false_type, std::true_type) {
  // see https://stackoverflow.com/questions/11980292/how-to-wrap-around-a-range
  return x - std::floor(x / y) * y;
}

/*!
 * \brief positiveModulo performs a modulo operation which is defined for
 * both integer and floating point types. The definition is not the same as the
 * definition of the %operator, the result is the same for positive integer
 * operands,
 * but different for negative operands (% is not defined for floating point
 * operands) (positiveModulo result has same sign as the divisor, % result has
 * same sign as dividend).
 * see https://en.wikipedia.org/wiki/Modulo_operation
 * \param x the dividend.
 * \param y the divisor.
 * \return x modulo y
 */
template <typename T1, typename T2>
inline auto positiveModulo(T1 x, T2 y) {
  return positiveModulo_detail(
      x,
      y,
      typename std::integral_constant < bool,
      std::is_integral<T1>::value&& std::is_integral<T2>::value > (),
      typename std::integral_constant < bool,
      std::is_floating_point<T1>::value || std::is_floating_point<T2>::value > ());
}

/*!
 * \brief Wraps the given angle into the range [-Pi, Pi).
 * \param angle The angle to wrap.
 * \return The wraped angle.
 */
template <typename T>
inline T wrapAngleMinusPiToPi(T angle) {
  static_assert(
      std::is_floating_point<T>::value,
      "you can only wrap floating point angles (float, double, long double)");
  return positiveModulo(angle + M_PI, 2 * M_PI) - M_PI;
}

/*!
 * \brief Wraps the given angle into the range [-given-Pi, given+Pi).
 * \param angle The angle to wrap.
 * \param given The angle to wrap around.
 * \return The wraped angle.
 */
template <typename T>
inline T wrapAngleAroundGivenAngle(T angle, T given) {
  static_assert(
      std::is_floating_point<T>::value,
      "you can only wrap floating point angles (float, double, long double)");
  return positiveModulo(angle - given + M_PI, 2 * M_PI) - M_PI + given;
}

/*!
 * \brief Returns the closest angle to given goal-angle in 2*n*pi distance to
 * given angle.
 * \param angle The angle to normalize relative to goal.
 * \param goal The angle to be as close as possible to.
 * \return the normalized angle.
 */
template <typename T>
inline T findCloseAngle(T angle, T goal) {
  static_assert(
      std::is_floating_point<T>::value,
      "you can only find close angle of (float, double, long double)");
  if (angle - goal > M_PI) {
    unsigned int num = (angle - goal - M_PI) / (2 * M_PI) + 1;
    return angle - 2 * M_PI * num;
  } else if (angle - goal < -M_PI) {
    unsigned int num = (goal - angle - M_PI) / (2 * M_PI) + 1;
    return angle + 2 * M_PI * num;
  } else {
    return angle;
  }
}

/*!
 * \brief Wraps the given angle into the range [min, max). Given angle will be
 * overwritten! Example 1: angle = 361, min = -160, max = 100
 * ->angle = 1, true
 * Example 2: angle = 361, min = -160, max = 0
 * ->angle = 361, false
 * Example 3: angle = 361, min = -160, max = 0
 * ->angle = 361, false
 * \param angle The angle to wrap.
 * \param min The lower bound of the wrapping
 * \param max The higher bound of the wrapping
 * \return True, if the angle could be wrapped inside min and max
 */
template <typename T>
inline bool wrapAngleBetweenMaxMin(T& angle, const T min, const T max) {
  const double span = max - min;
  const double offset = min + span / 2.0;
  // std::fmod(angle, 2.0 * M_PI);
  // returns modulo value in range [0; 2pi)

  // std::fmod(angle + M_PI, 2.0 * M_PI) - M_PI;
  // returns modulo value in range [-pi; pi)

  // std::fmod(angle - offset + M_PI, 2.0 * M_PI) + offset - M_PI;
  // returns modulo value in range [-pi + offset; pi + offset)

  // we shift the problem to wrap around "min <= x < max" to "0 <= x < pi"

  const double result = positiveModulo(angle - offset + M_PI, 2.0 * M_PI) + offset - M_PI;

  // check if the wrap was valide. E.g. we could not wrap angle = 2 between min = -1 and max 0 1 since  angle-2*pi < min < max < angle
  if (max >= result && result >= min) {
    angle = result;
    return true;
  }
  return false;
}

/*!
 * \brief Wraps the given angle into the range [0, 2*Pi).
 * \param angle The angle to wrap.
 * \return The wraped angle.
 */
template <typename T>
inline T wrapAngleZeroToTwoPi(T angle) {
  static_assert(
      std::is_floating_point<T>::value,
      "you can only wrap floating point angles (float, double, long double)");
  return positiveModulo(angle, 2 * M_PI);
}

/*!
 * \brief Returns true if  min < x < max
 * \param x The value to check
 * \param max Upper border
 * \param min Lower border
 */
template <typename T>
inline bool isBetween(T x, T max, T min) {
  return (max > x && x > min);
}
/*!
 *\brief ComparableAngle changes two given angles such that they are in a comparable range.
 */
template <typename T>
inline void comparableAngle(T& a, T& b) {
  if (a < b) {
    if (b - a > M_PI) {
      b = b - 2 * M_PI;
      comparableAngle(a, b);
    }
  } else {
    if (a - b > M_PI) {
      a = a - 2 * M_PI;
      comparableAngle(a, b);
    }
  }
}


/*! \brief almost_equal checks if two floating points differ in epsilon
 * https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 * \param a,b floating point numbers checked to be within epsilon
 * \psrsm ulp (units in the last place of precision) ((GTest uses 4 ULP's for
 * its EXPECT_DOUBLE_EQ))
 * \return true if abs(a-b) < epsilon
 */
template <class T>
inline typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type almost_equal(
    T x, T y, int ulp) {
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
         // unless the result is subnormal
         || std::fabs(x - y) < std::numeric_limits<T>::min();
}



}  // namespace math

#endif  // !_SIMULATEDUNIT_
