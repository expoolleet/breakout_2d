#if defined(__has_cpp_attribute) && __has_cpp_attribute(clang::no_destroy)
#define NO_DESTROY_ATTR [[clang::no_destroy]]
#else
#define NO_DESTROY_ATTR
#endif
