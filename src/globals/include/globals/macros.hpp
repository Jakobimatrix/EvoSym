#ifndef MACROS
#define MACROS

#include <string>
#include <ostream>

#ifdef NDEBUG
  inline void DEBUG(std::string_view s){
    std:: cout << "DEBUG: \t" << s << "\n";
  }

  inline void WARNING(std::string_view s){
    std:: cout << "WARNING: \t" << s << "\n";
  }

  inline void ERROR(std::string_view s){
    std:: cout << "ERROR: \t" << s << "\n";
  }

#else
  inline void DEBUG(std::string_view s){}
  inline void WARNING(std::string_view s){}
  inline void ERROR(std::string_view s){}
#endif

#endif
