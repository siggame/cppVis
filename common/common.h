#ifndef COMMON_H
#define COMMON_H

#include "exception.h"
#include "smartpointer.h"
#include <QtOpenGL>
#include <QMessageBox>
#include <sstream>

#if __DEBUG__

#include <stdio.h>
#include <iostream>

#include <map>


typedef struct {
  long int address;
  long int size;
  char  file[64];
  long int line;

} ALLOC_INFO;

extern list< ALLOC_INFO > allocMap;

void DumpUnfreed();

#ifdef __DEBUG_NEW__

void AddTrack(long int addr,  long int asize,  const char *fname, long int lnum);
void RemoveTrack( long int addr );

inline void *operator new( size_t size, const char *file, int line )
{
  void *ptr = (void*)malloc( size );
  AddTrack( (unsigned long)ptr, size, file, line );
  return ptr;

}

inline void *operator new [] ( size_t size, const char *file, int line )
{
  void *ptr = (void*)malloc( size );
  AddTrack( (unsigned long)ptr, size, file, line );
  return ptr;

}

inline void operator delete( void *p )
{
  RemoveTrack( (long int)p );
  free( p );

}

inline void operator delete [] ( void *p )
{
  RemoveTrack( (long int)p );
  free( p );

}

#define DEBUG_NEW new( __FILE__, __LINE__ )
#define DEBUG_DELETE( x ) Delete( x )

#define new DEBUG_NEW

#endif

#endif

namespace visualizer 
{

#define THROW( x, y, ... ) \
  { \
    char message[2048]; \
    sprintf( message, y, ##__VA_ARGS__ ); \
    QMessageBox::critical( 0, "Critical!", message );\
    throw x( message, __FILE__, __LINE__ ); \
  } 

#define WARNING( y, ... ) \
  { \
    char message[2048]; \
    stringstream ss; \
    sprintf( message, y "\nFile: %s \nLine: %d", ##__VA_ARGS__, __FILE__, __LINE__ ); \
    ss << "WARNING: " << message << endl; \
    cerr << ss.str(); \
    errorLog << ss.str(); \
    QMessageBox::critical( 0, "Warning!", message );\
  } 

#define MESSAGE( y, ... ) \
  { \
    char message[2048]; \
    stringstream ss; \
    sprintf( message, y, ##__VA_ARGS__ ); \
    ss << "MESSAGE: " << message << endl; \
    cerr << ss.str(); \
    ss << "File: " << __FILE__ << " Line: " << __LINE__ << endl; \
    errorLog << ss.str(); \
  }

#if __DEBUG__

#define SETUP(x) MESSAGE( "%s initialized. ", x )

  inline void printStackTrace() 
  {
#ifdef __STACKTRACE__ 
    // Exception is closing everything down anyway.  May as well use up some memory
    void *array[256];
    size_t size;
    cerr << " Stack Trace: " << endl;
    size = backtrace( array, 10 );
    backtrace_symbols_fd( array, size, 2 );
#endif
  }
  inline void openglErrorCheck()
  {
    unsigned int err;
    while( (err = glGetError() ) )
    {
      switch( err )
      {
        case GL_INVALID_ENUM:
          {
            WARNING( "Invalid Enumeration Used In Some OpenGL Thing"  );
          } break;
        case GL_INVALID_VALUE:
          {
            WARNING( "An Invalid Value Was Used In Some OpenGL Argument And Was Ignored.  Wish I had more details for you..." );
          } break;
        case GL_INVALID_OPERATION:
          {
            WARNING( "An Invalid OpenGL Operation Was Performed and Ignored.  Wish I had more details for you..." );
          } break;
        case GL_STACK_OVERFLOW:
          {
            WARNING( "An OpenGL Comman You Tried to Perform Would Have Caused A Stack Overflow.  It was ignored.  Wish I had more details for you..." );
          } break;
        case GL_STACK_UNDERFLOW:
          {
            WARNING( "WHAT THE FUCK DID YOU DO!?!?" );
          } break;
        case GL_OUT_OF_MEMORY:
          {
            THROW( OpenGLException, "OpenGL Is Out Of Memory.  Clean up after yourself." );
          } break;
        case GL_NO_ERROR:
          break;
        default:
          THROW( OpenGLException, "OpenGL Error Occurred Somewhere." );

      }
    }

  }

#define IMPLEMENT_ME THROW( Exception, "This Feature Is Not Yet Implemented." );

#define DBG_MSG( x ) std::cout << "DEBUG MSG: " << x << endl;

#else

#define SETUP(x)
#define DBG_MSG( x )

#endif                           /* __DEBUG__ */

  class Module
  {
    public:
      static void setup();
      static void destroy();
      virtual ~Module() {};
  };

} // visualizer

#endif
