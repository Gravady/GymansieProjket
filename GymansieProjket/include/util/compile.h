#ifndef COMPILE_H
#define COMPILE_H

#define DISPLAYERROR(msg) \
    __pragma(message("ERROR with " msg " in file " __FILE__ " at " __TIME__ " on " __DATE__ \
                     ", C++ version: " TOSTRING(__cplusplus))) \

#define DISPLAYWARNING(msg) \
    __pragma(message("Warning with " msg " in file " __FILE__ " at " __TIME__ " on " __DATE__ \
                     ", C++ version: " TOSTRING(__cplusplus)))

#define DISPLAYMESSAGE(msg) \
    __pragma(message(msg " in file " __FILE__ " at " __TIME__ " on " __DATE__)) \

constexpr void assertCompilerEncoding() {
    static_assert(L'☃' == L'\x2603', "UTF-8 not found");
    DISPLAYERROR("Compiler does not support UTF-8");
}

#endif