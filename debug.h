#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef NDEBUG
  // No debug
  #define debug( fmt, ... ) ((void)0)
#else /* !NDEBUG */
  // on debug
  #include <stdio.h>
  #define debug( fmt, ... ) \
        fprintf( stderr, "[%s:%d] " fmt "", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif /* NDEBUG */
#endif /* _DEBUG_H_ */
