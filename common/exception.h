#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#ifdef __GNUC__ 
#ifndef __MINGW32__
#include <execinfo.h>
#define __STACKTRACE__
#endif
#endif

using namespace std;

namespace visualizer
{
  class Log
  {
    public:
      Log( const std::string& fileName )
        : out_file( fileName.c_str() )
      {
      };

      ~Log()
      {
      }

      Log& operator << ( const std::string& line )
      {
        return write( line );
      }

      Log& write( const std::string& line )
      {
        out_file << line << endl;
        return *this;
      }

    private:
      std::ofstream out_file;
  };

  extern Log* errorLog;


  class Exception
  {
    public:
      Exception()
      {
      }

      Exception( std::string e, std::string fileName, unsigned int lineNum )
      {
        printException( e, fileName, lineNum, "General Exception" );
      }

      virtual ~Exception() {}

      void printException
        ( 
         std::string e, 
         std::string fileName, 
         unsigned int lineNum, 
         std::string exceptionType 
        )
      {
        stringstream ss;

        ss << endl;
        ss << "=================== " << exceptionType << " =================== " << endl;
        ss << " Error: " << e << endl;
        ss << " File:  " << fileName << ":" << lineNum << endl;

        cerr << ss.str();
        errorLog->write(ss.str());

#ifdef __STACKTRACE__ 
        // Exception is closing everything down anyway.  May as well use up some memory
        void *array[256];
        size_t size;
        cerr << " Stack Trace: " << endl;
        size = backtrace( array, 10 );
        backtrace_symbols_fd( array, size, 2 );
#endif
      }
  };

  class GameException : public Exception
  {
    public:
      GameException( std::string e, std::string fileName, unsigned int lineNum )
      {
        printException( e, fileName, lineNum, "Game Loading Exception" );
      }
  };

  class FileException : public Exception
  {
    public:
      FileException( std::string e, std::string fileName, unsigned int lineNum )
      {
        printException( e, fileName, lineNum, "File Exception" );
      }
  };

  class OpenGLException : public Exception
  {
    public:
      OpenGLException( std::string e, std::string fileName, unsigned int lineNum )
      {
        printException( e, fileName, lineNum, "OpenGL Error Detected" );
      }
  };

} // visualizer

#endif
