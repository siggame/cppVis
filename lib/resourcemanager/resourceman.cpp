#include "resourceman.h"
#include "textureloader.h"
#include "renderer/text.h"
#include "resource.h"

#include <sstream>
#include <fstream>
#include <string>
#include <QFileInfo>

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
     const std::string& path, 
     const std::string& name
    )
  {
    ifstream fin( path.c_str() );
    std::string tag;
    std::vector<std::string> attributes;
    std::vector<ResShader::Uniform> uniforms;
    std::vector<ResShader::ShaderInfo> shaderInfo;
    QFileInfo fInfo(path.c_str());

    if(fin.fail())
        return;

    // because HELL yes
    if(fInfo.suffix() != "megashader")
    {
        fin.close();
        return;
    }

    while(fin >> tag)
    {
        if(tag == "input")
        {
            fin >> tag;
            uint size = Renderer->shaderTypeSize(tag);
            fin >> tag;
            // ensuring that it is a recognized type, and an acceptable
            // vertex attrib. Must conform to my naming standard for attribs
            // to ensure shader compatability
            if(size != 0 && Renderer->vertexAttribSize(tag))
            {
                attributes.push_back(tag);
            }
        }
        else if(tag == "uniform")
        {
            std::string type;
            fin >> type;
            uint size = Renderer->shaderTypeSize(type);
            fin >> tag;
            if(size != 0)
            {
                ResShader::Uniform u;
                u.type = type;
                u.name = tag;
                uniforms.push_back(u);
            }
        }
        else if(tag == "shader")
        {
            std::string type;
            fin >> type;
            fin >> tag;
            if(type == "f" || type == "v")
            {
                ResShader::ShaderInfo info;
                info.type = type;
                info.filename = (fInfo.path() + "/" + tag.c_str()).toStdString();
                std::cout << info.filename << std::endl;
                shaderInfo.push_back(info);
            }
        }

        if(!fin.good()) // if it unexpectedly hit the end, or something else went awry
        {
            fin.close();
            return;
        }
    }

    fin.close();

    uint shaderProgramID = Renderer->createShader(attributes, shaderInfo);

    if(shaderProgramID != 0)
    {
        ResShader* res = new ResShader(attributes, shaderProgramID);
        reg(name, res);
    }
  }

  void _ResourceMan::loadModel
    (
        const string& path,
        const string& name
    )
  {
      ifstream fin ( path.c_str(), std::ifstream::in | std::ifstream::binary);
      QFileInfo fInfo(path.c_str());
      std::vector<ResModel::Attrib> attribs;
      std::vector<ResModel::TexInfo> textures;
      char* rawVertBuffer = nullptr;
      unsigned int* rawIndexBuffer = nullptr;
      std::vector<ResModel::Bone> bones;
      std::vector<ResModel::Animation> animations;

      unsigned int attribSize = 0;
      int currentOffset = 0;           // also the size of a complete vertex after attribs are found
      int blockSize = 0;               // number of elements in the section (NOT BYTES)


      if(fin.fail())
          return;

      // because HELL yes
      if(fInfo.suffix() != "megamodel")
      {
          fin.close();
          return;
      }

      // attribs
      fin.read((char*) &blockSize, sizeof(blockSize));
      for(int i = 0; i < blockSize; i++)
      {
          ResModel::Attrib newAttrib;
          fin >> newAttrib.name;
          attribSize = Renderer->vertexAttribSize(newAttrib.name);
          if(attribSize != 0)
          {
            newAttrib.offset = currentOffset;
            currentOffset += attribSize;
            attribs.emplace_back(newAttrib);
          }
      }

      // textures
      fin.read((char*) &blockSize, sizeof(blockSize));
      for(int i = 0; i < blockSize; i++)
      {
          ResModel::TexInfo newTexInfo;
          fin >> newTexInfo.texCoord;
          if(newTexInfo.texCoord != "tex1" || newTexInfo.texCoord != "tex2")
          {
              fin >> newTexInfo.fileName;
              textures.emplace_back(newTexInfo);
          }
      }

      // vertices
      fin.read((char*) &blockSize, sizeof(blockSize));
      rawVertBuffer = new char[blockSize * currentOffset];
      fin.read(rawVertBuffer, blockSize * currentOffset);

      // indices
      fin.read((char*) &blockSize, sizeof(blockSize));
      rawIndexBuffer = new unsigned int[blockSize];
      fin.read((char*) rawIndexBuffer, blockSize * sizeof(unsigned int));

      // bones (where it gets tricky)
      fin.read((char*) &blockSize, sizeof(blockSize));
      for(int i = 0; i < blockSize; i++)
      {
          // each bone has name and a 4x4 (16 float) matrix describing it's tranlation from the
          // local origin of the model
          ResModel::Bone newBone;
          fin >> newBone.name;

          // stored in column major in the file
          for(int j = 0; j < 4; j++)
              for(int k = 0; k < 4; k++)
                  fin.read((char*) &newBone.offset[j][k], sizeof(newBone.offset[j][k]));

          bones.emplace_back(newBone);
      }

      // animations (the most difficult and final part)
      fin.read((char *) &blockSize, sizeof(blockSize));
      for(int i = 0; i < blockSize; i++)
      {
          // because these can become so large, rather than make a temp anim, filling it up, and
          // copying it into a vector, I construct it within the the vector and then
          // get a reference to the object I just made.
          animations.emplace_back(ResModel::Animation());
          ResModel::Animation & newAnim = animations.back();

          // each animation contains a name, total duration in ticks,
          // and the number of ticks per second.
          fin >> newAnim.name;
          fin.read((char*) &newAnim.duration, sizeof(newAnim.duration));
          fin.read((char*) &newAnim.ticksPerSec, sizeof(newAnim.ticksPerSec));

          // next is the number of nodes (or bones) that change over the course of the animation
          // these are sometimes referred to as channels, so this would be the number of channels
          fin.read((char*) &blockSize, sizeof(blockSize));

          // next is a biggie. For each node (channel) there will be set of scalings that take place
          // over the course of the animation (in the form of a 3 float vector), a set of rotations
          // (in the form of a 4 float quaternion), and a set of translations (also in the form of a
          // 3 float vector). Here is an example of a 2 second animation on a single node:
          // each '-' is a tick    ticks = 50     tickPerSec = 25
          //     start                                               end
          //       |                                                  |
          //       V                                                  V
          //----------------------------------------------------------------------
          //scale               *                                *
          //rotate                          *      *
          //trans         *                                      *
          //----------------------------------------------------------------------
          //
          // would look like this in my file:
          //             value of   num of     value of     num of   value of
          //  numScales  scale      Rotations  rotate       trans    trans
          //      V        V            V        V            V       V
          //     [2] [13] [*] [46] [*] [2] [25] [*] [32] [*] [2] [7] [*] [46] [*]
          //          ^                     ^                     ^
          //       time of                time of               time of
          //      this scale             rotation               trans
          //
          // and there is one of these for every node in the animations (usually one for
          // each bone to describe it's movement over the course of the animation, but if
          // the bone never moves, the node (channel) may be ommited)

          // for each node (or channel)
          for(int i = 0; i < blockSize; i++)
          {
              std::string name;

              // get the name of the bone this node is concerned with
              fin >> name;

              // an individual node gets pretty large, so i construct it in the map, and then
              // get a reference to that node in the map;
              newAnim.nodes[name] = ResModel::Animation::Node();
              ResModel::Animation::Node & newNode = newAnim.nodes[name];

              // get the number of scaling keys over the course of this animation for this bone
              fin.read((char*) &blockSize, sizeof(blockSize));
              for(int i = 0; i < blockSize; i++)
              {
                  ResModel::Animation::Node::VectorKey newScaleKey;

                  // NOTE: not quite sure if glm::vec3 is exactly 12 bytes laid out
                  // sequentially so i do them seperately
                  fin.read((char*) &newScaleKey.transform.x, sizeof(newScaleKey.transform.x));
                  fin.read((char*) &newScaleKey.transform.y, sizeof(newScaleKey.transform.y));
                  fin.read((char*) &newScaleKey.transform.z, sizeof(newScaleKey.transform.z));

                  fin.read((char*) &newScaleKey.time, sizeof(newScaleKey.time));

                  newNode.scales.emplace_back(newScaleKey);
              }

              fin.read((char*) &blockSize, sizeof(blockSize));
              for(int i = 0; i < blockSize; i++)
              {
                  ResModel::Animation::Node::QuatKey newRotationKey;

                  fin.read((char*) &newRotationKey.transform.x, sizeof(newRotationKey.transform.x));
                  fin.read((char*) &newRotationKey.transform.y, sizeof(newRotationKey.transform.y));
                  fin.read((char*) &newRotationKey.transform.z, sizeof(newRotationKey.transform.z));
                  fin.read((char*) &newRotationKey.transform.w, sizeof(newRotationKey.transform.w));

                  fin.read((char*) &newRotationKey.time, sizeof(newRotationKey.time));

                  newNode.rotations.emplace_back(newRotationKey);
              }

              fin.read((char*) &blockSize, sizeof(blockSize));
              for(int i = 0; i < blockSize; i++)
              {
                  ResModel::Animation::Node::VectorKey newTranslateKey;

                  fin.read((char*) &newTranslateKey.transform.x, sizeof(newTranslateKey.transform.x));
                  fin.read((char*) &newTranslateKey.transform.y, sizeof(newTranslateKey.transform.y));
                  fin.read((char*) &newTranslateKey.transform.z, sizeof(newTranslateKey.transform.z));

                  fin.read((char*) &newTranslateKey.time, sizeof(newTranslateKey.time));

                  newNode.translations.emplace_back(newTranslateKey);
              }
          }
      }

      // that should be everything in the file. Now we need to set up the mesh in OpenGL.

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
    ifstream metaIn( metaData.c_str() );
    metaIn >> frames; // todo: this var could be removed
    metaIn >> width;
    metaIn >> height; 


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

