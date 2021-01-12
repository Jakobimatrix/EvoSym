#ifndef DISP_UTILS_H
#define DISP_UTILS_H

#include <QOpenGLFunctions>
#include <globals/macros.hpp>
#include <string>

namespace disp_utils {
/*!
 * \brief This prints out the last GL errors.
 * \param file The file name (use __FILE__)
 * \param line The line (use __LINE__)
 * \param expression The function call to GL
 * usage: glCheck(FUN2GL(val1, val2...));
 */
inline void checkError(const char* file, unsigned int line, const char* expression) {
  GLenum e;
  do {
    e = glGetError();
    std::string error;
    if (e == GL_NO_ERROR) {
      return;
    } else if (e == GL_INVALID_ENUM) {
      error = "An unacceptable value is specified for an enumerated argument.";
    } else if (e == GL_INVALID_VALUE) {
      error = "A numeric argument is out of range.";
    } else if (e == GL_INVALID_OPERATION) {
      error = "The specified operation is not allowed in the current state. ";
    } else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) {
      error = "The framebuffer object is not complete.";
    } else if (e == GL_OUT_OF_MEMORY) {
      error = "There is not enough memory left to execute the command.";
    } else if (e == GL_STACK_UNDERFLOW) {
      error =
          "An attempt has been made to perform an operation that would "
          "cause an internal stack to underflow.";
    } else if (e == GL_STACK_OVERFLOW) {
      error =
          "An attempt has been made to perform an operation that would "
          "cause an internal stack to overflow.";
    } else {
      error = "An unknown error " + std::to_string(e) + " occured.";
    }
    F_ERROR(
        "\nAn OpenGL call failed in %s Line: %u \nFunction: %s "
        "\nError "
        "description: %s\n",
        file,
        line,
        expression,
        error.c_str());

  } while (e != GL_NO_ERROR);
}

}  // namespace disp_utils


#ifdef NDEBUG
#define glCheck(expr) expr
#define glCheckAfter()
#else
#define glCheck(expr) \
  expr;               \
  disp_utils::checkError(__FILE__, __LINE__, #expr);

#define glCheckAfter() \
  disp_utils::checkError(__FILE__, __LINE__, "Somewhere above this.");
#endif

namespace disp_utils {

// Rather then using the recomended glBindAttribLocation prior to linking the
// shader I did that here, which is bad according to some. but it works for
// sfml shaders too which are linked somewhere deep in sfml.
// also since I am stuck with #version 130 (GLSL 1.30)
// I cannot use layout(location = 0) which is avaiable in GLSL 1.40
inline void assignShaderVariable(unsigned int shaderProgram,
                                 const char* var_name,
                                 int num_values,
                                 int stride,
                                 void* start_position,
                                 QOpenGLFunctions* gl) {
  const int variable_position = gl->glGetAttribLocation(shaderProgram, var_name);
  glCheckAfter();
  if (variable_position < 0) {
    // compiler did optimize away the variable
    F_WARNING(
        "Trying to connect to shader variable %s failed. Variable not "
        "found",
        var_name);
    return;
  }
  const unsigned int u_pos = static_cast<unsigned int>(variable_position);
  glCheck(gl->glEnableVertexAttribArray(u_pos));
  glCheck(gl->glVertexAttribPointer(
      u_pos, num_values, GL_FLOAT, GL_FALSE, stride, start_position));
  /*
  F_DEBUG(
      "connecting %s with %d values starting at %p. Connected Position is "
      "%d",
      var_name,
      num_values,
      start_position,
      u_pos);*/
}
}  // namespace disp_utils

#endif
