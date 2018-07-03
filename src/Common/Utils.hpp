#ifndef _UTILS_HPP_
#define _UTILS_HPP_


#define didemo_dbug
#ifdef didemo_dbug
#define DPRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DPRINT(fmt, ...) do {} while (0)
#endif

#define _FUNCTION_E_ DPRINT("%s %s E:\n", __FILE__,__FUNCTION__)
#define _FUNCTION_X_ DPRINT("%s %s X:\n", __FILE__,__FUNCTION__)
#endif