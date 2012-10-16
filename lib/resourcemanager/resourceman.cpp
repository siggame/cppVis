#include "resourceman.h"
#include "textureloader.h"
#include "renderer/text.h"
#include "resource.h"

#include <sstream>
#include <fstream>
#include <string>

using namespace std;

namespace visualizer
{

  _ResourceMan *ResourceMan = 0;

  std::vector<ResID_t> _ResourceMan::listResourceNames()
  {
    std::vector<ResID_t> names;
    DataTable::iterator it = data()->begin();
    for (; it != data()->end(); it++)
    {
      names.push_back(it->first);
    }
    return names;
  }


  bool _ResourceMan::exists(const ResID_t & rName)
  {
    return ManagerType::exists(rName);
  }


  bool _ResourceMan::del(const ResID_t & rName)
  {
    if (!exists(rName))
    {
      return false;
    }

    DataTable * dt = data();
    Resource * ref = (*(dt))[rName];
    if (ref->numReferences())
    {
#ifdef __DEBUG__
      WARNING( "Resource \"%s\" still has a reference", rName.c_str() );
#endif
      return false;
    }

    return delPointer(rName);
  }


  void _ResourceMan::destroy()
  {

    DataTable::iterator it = ResourceMan->data()->begin();
    for (; it != ResourceMan->data()->end(); it++)
    {

      //if( it->second )
      if (it->second->numReferences())
      {
        THROW( Exception, "Resource Manager still contains some references" );
      }

      //if( it->second )
      if (!it->second->unload())
        THROW( Exception, "Problem Unloading ResourceManager" );

      delete it->second;
      it->second = 0;

    }

    delete ResourceMan;
    ResourceMan = 0;

  }


  Resource * _ResourceMan::reference(const std::string & rName, const std::string & referencer)
  {
    Resource ** res = ManagerType::getItem(rName);
    if (!res)
    {
      THROW
        (
         Exception,
         "Resource Could Not Be Found: \n  %s", rName.c_str()
        );
      return NULL;
    }

    if (!*res)
    {
      THROW
        (
         Exception,
         "Resource Could Not Be Found: \n  %s", rName.c_str()
        );
      return NULL;
    }

    if ((*res)->reference(referencer))
      return *res;

    THROW
      (
       Exception,
       "Could not retrieve resource because it is already in use.  Please make sure \
       that you have released the resource once you're done.  This also occurs, when \
       you do multithreading."
      );

    return NULL;

  }


  void _ResourceMan::setup()
  {
    if( !ResourceMan )
    {
      ResourceMan = new _ResourceMan;
    }

  }


  bool _ResourceMan::release(const std::string &rName, const std::string &referencer)
  {
    Resource ** res = ManagerType::getItem(rName);

    if (!res)
      return false;

    if (!*res)
      return false;

    if ((*res)->deReference(referencer))
      return true;

    return false;

  } // _ResourceMan::release()

  void _ResourceMan::loadShader
    (
     const size_t& type, 
     const std::string& path, 
     const std::string& name
    )
  {

    ifstream shader( path.c_str() );
    
    if( shader.is_open() )
    {
      std::string shaderSource((std::istreambuf_iterator<char>(shader)), std::istreambuf_iterator<char>());
      
      unsigned int id = glCreateShader(type);

      const char *source = shaderSource.c_str();
      glShaderSource( id, 1, &source, 0 );
      glCompileShader( id );

      int status;
      glGetShaderiv( id, GL_COMPILE_STATUS, &status );

      if( !status )
      {
        WARNING( "Shader '%s' did not compile correctly", name.c_str() );
      }

      int logLength;
      glGetShaderiv( id, GL_INFO_LOG_LENGTH, &logLength );

      char *log = new char[logLength+1]; 
      glGetShaderInfoLog( id, logLength, 0, log );
      MESSAGE( "'%s' info log: \n %s", name.c_str(), log );
      delete [] log;
      ResShader *res = new ResShader(id);
      reg( name, res );

    } else
    {
      WARNING( "Shader could not be found: %s", path.c_str() );
    }

  }

  void _ResourceMan::loadFont
    ( 
     const std::string& fontWidths, 
     const std::string& fontTexture, 
     const std::string& name 
    )
    {
      static size_t fontNum = 0;
      std::stringstream fontName;
      fontName << "MEGAMINERVISUALIZER_UNIQUEFONTNAME_" << fontNum++;
      loadTexture( fontTexture, fontName.str() );
      Text t( fontName.str(), fontWidths );

      Resource *res = (Resource*)( new ResFont( t ) );
      reg( name, res );

    } // _ResourceMan::loadFont()

  void _ResourceMan::loadTexture
    ( 
     const std::string& filename, 
     const std::string& name 
    )
    {
      Resource * res = 0;
      QImage texture;
      int id = 0;

      if( ( id = TextureLoader->load( filename.c_str(), texture ) ) )
      {
        res = (Resource*)( new ResTexture( texture, id ) );
        reg( name, res );
      }
      else
      {
        THROW
          (
           Exception,
           "Texture: '%s' did not load correctly", filename.c_str()
          );
      }

    } // _ResourceMan::loadTexture()

  void _ResourceMan::loadAnimation( const std::string& filename, const std::string& name )
  {
    string discard;
    Resource * res = NULL;
    QImage texture;
    int id = 0;
    size_t frames, width, height;

    const std::string metaData = filename + ".meta";
    cout<<metaData<<endl;
    ifstream metaIn( metaData.c_str() );
    metaIn >> frames;
    metaIn >> width;
    metaIn >> height;
    
    cout<<frames<<endl;
    cout<<width<<endl;
    cout<<height<<endl;

    if( ( id = TextureLoader->load( filename.c_str(), texture ) ) )
    {
      res = (Resource*)( new ResAnimation( texture, id, width, height, frames ) );
      reg( name, (Resource*)res );
    }
    else
    {
      THROW
        (
         Exception,
         "Animation '%s' did not load correctly", filename.c_str()
        );
    }

  }

  void _ResourceMan::loadTexture( QImage& image, const std::string& name )
  {
    unsigned int id;
    Resource * res = NULL;

    TextureLoader->loadQImage( id, image );

    res = (Resource*)(new ResTexture( image, id ));
    reg( name, (Resource*)res );

  } // _ResourceMan::loadTexture()

  void _ResourceMan::newDisplayList( const std::string& name )
  {
    Resource* res = (Resource*)new ResDisplayList();
    reg( name, res );

  } // _ResourceMan::newDisplayList()


} // visualizer

