#ifdef ENABLE_STRING_CHECK
# define strncpy(dst, src, len)            strncpy_safe(dst, src, len, __FILE__, __LINE__)
# define snprintf(buf, len, fmt, args...)  snprintf_safe(buf, len, __FILE__, __LINE__, fmt,## args)
# define vsnprintf(buf, len, fmt, args)    vsnprintf_safe(buf, len, __FILE__, __LINE__, fmt, args)
# define strncat(dst, src, len)            strncat_safe(dst, src, len, __FILE__, __LINE__)
# ifdef ENABLE_CHECK_ALL
#  define sprintf       do_not_use_sprintf
#  define vsprintf      do_not_use_vsprintf
#  define strcpy        do_not_use_strcpy
#  define strcat        do_not_use_strcat
# endif
#endif

#ifdef DISABLE_STRING_CHECK
# undef  strncpy
# undef  snprintf
# undef  vsnprintf
# undef  sprintf
# undef  vsprintf
# undef  strcpy
# undef  strcat
#endif


#undef DISABLE_STRING_CHECK
#undef ENABLE_STRING_CHECK
#undef ENABLE_CHECK_ALL

