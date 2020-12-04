#ifndef DISP_UTILS_H
#define DISP_UTILS_H

#include <glad/glad.h>

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
        "\nAn OpenGL call failed in Line: %u %s \nFunction:\n %s "
        "\nError "
        "description: %s\n",
        line,
        file,
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

#endif
