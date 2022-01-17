#pragma once

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

// 
#ifdef memmove
#undef memmove
#endif

#ifdef wmemmove
#undef wmemmove
#endif

#ifdef memcpy
#undef memcpy
#endif

#ifdef wmemcpy
#undef wmemcpy
#endif

#ifdef memset
#undef memset
#endif

#ifdef wmemset
#undef wmemset
#endif