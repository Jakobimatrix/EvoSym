#ifndef MACROS
#define MACROS

#include <string>
#include <stdio.h>

#ifdef NDEBUG

#define DEBUG(str)
#define F_DEBUG(fmt, ...)
#define WARNING(str)
#define F_WARNING(fmt, ...)
#define ERROR(fmt, ...)
#define F_ERROR(fmt, ...)

#else

#define RED(string) "\x1b[31m" string "\x1b[0m"
#define ORANGE(string) "\033[38:5:208:0m" string "\033[0m"

//https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
#define DEBUG(str) \
        fprintf(stderr, "[DEBUG]\t %s::%s()  Line: %d: %s\n", __FILE__, \
            __func__, __LINE__, str)

#define F_DEBUG(fmt, ...) \
        do { fprintf(stderr, "[DEBUG]\t %s::%s()  Line: %d: " fmt, __FILE__, \
            __func__, __LINE__, __VA_ARGS__); } while (0); \
            printf("\n")

#define WARNING(str) \
        fprintf(stderr, ORANGE("[WARN]")"\t %s::%s()  Line: %d: %s\n", __FILE__, \
            __func__, __LINE__, str)

#define F_WARNING(fmt, ...) \
        do { fprintf(stderr, ORANGE("[WARN]")"\t %s::%s()  Line: %d: " fmt, __FILE__, \
            __func__, __LINE__, __VA_ARGS__); } while (0); \
            printf("\n")

#define ERROR(str) \
        fprintf(stderr, RED("[ERROR]")"\t %s::%s()  Line: %d: %s\n", __FILE__, \
            __func__, __LINE__, str)

#define F_ERROR(fmt, ...) \
        do { fprintf(stderr, RED("[ERROR]")"\t %s::%s()  Line: %d: " fmt, __FILE__, \
            __func__, __LINE__, __VA_ARGS__); } while (0); \
            printf("\n")
#endif

#endif
