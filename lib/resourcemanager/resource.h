#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <QImage>
#include <map>
#include "glew/glew.h"
#include <GL/gl.h>
#include "common.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "interfaces/iresourceman.h"
#include "renderer/text.h"

#ifdef DEBUG
#include <iostream>
#endif

namespace visualizer
{

  struct Coord
  {
    Coord()
    {
      x = y = z = 0;
    }

    Coord( const float& X, const float& Y, const float& Z = 0.0f )
    {
      x = X;
      y = Y;
      z = Z;
    }

    float x;
    float y;
    float z;
  };

  struct Rect
  {
    Coord upLeft, upRight, bottomLeft, bottomRight;
  };

  class ResDisplayList: public Resource
  {
    public:
      ResDisplayList()
        : Resource( RT_DISPLAY_LIST )
      { listNum = glGenLists( 1 ); }

      void startList()
      {
        glNewList( listNum, GL_COMPILE );

      }

      void endList()
      {
        glEndList();
      }

      void draw()
      {
        glCallList( listNum );
      }

    private:
      unsigned int listNum;
  };

  class ResShader: public Resource
  {
    public:
      struct Uniform
      {
        std::string name;
        std::string type;
      };

      struct ShaderInfo
      {
          std::string type;
          std::string filename;
      };

    public:
      ResShader( std::vector<std::string>& attribs, const unsigned int& id) :
          Resource( RS_SHADER ),
          m_attribs(std::move(attribs)),
          m_shaderProgramID(id)
          {}

      const unsigned int& getShader() const { return m_shaderProgramID; }

      const std::vector<std::string>& getAttribs() const {return m_attribs;}

    private:
      std::vector<std::string> m_attribs;
      unsigned int m_shaderProgramID;
  };

  class ResModel : public Resource
  {
    public:
      struct Attrib
      {
          std::string name;
          unsigned int offset;
      };

      struct TexInfo
      {
          std::string texCoord;
          std::string fileName;
      };

      struct Bone
      {
          std::string name;
          glm::mat4 offset;
      };

      struct Animation
      {
          struct Node
          {
              struct VectorKey
              {
                  glm::vec3 transform;
                  double time;
              };

              struct QuatKey
              {
                  glm::quat transform;
                  double time;
              };

              std::vector<VectorKey> scales;
              std::vector<QuatKey> rotations;
              std::vector<VectorKey> translations;
          };

          std::string name;
          double duration;
          double ticksPerSec;
          std::map<std::string, Node> nodes;
      };

     public:
       ResModel(unsigned int vertexBuffer,
                unsigned int indexBuffer,
                std::vector<ResModel::Attrib>& attribs,
                std::vector<ResModel::TexInfo>& textures,
                std::vector<ResModel::Bone>& bones,
                std::vector<ResModel::Animation>& animations) :
           Resource(RS_MODEL),
           m_VertexBuffer(vertexBuffer),
           m_IndexBuffer(indexBuffer),
           m_Attribs(std::move(attribs)),
           m_Textures(std::move(textures)),
           m_Bones(std::move(bones)),
           m_Animations(std::move(animations))
           {}

       const unsigned int& getVertexBuffer() const {return m_VertexBuffer;}

       const unsigned int& getIndexBuffer() const {return m_IndexBuffer;}

       const Attrib* getAttribInfo
         (
           const std::string& attrib
         ) const;

       const unsigned int getVertexArray
         (
           const std::string& shader
         ) const;

       const std::string& getTexName
         (
           const std::string& texCoord,
           unsigned int skinIndex
         ) const;

       void addVertexArray
         (
           const std::string& shader,
           unsigned int vao
         );

       // TODO: needs to be a function to return a list of matrices for node transforms
       //       lot of math i will do in the next couple of weeks

     private:
       unsigned int m_VertexBuffer;
       unsigned int m_IndexBuffer;
       std::map<std::string, unsigned int> m_VertexArrays;

       std::vector<ResModel::Attrib> m_Attribs;
       std::vector<ResModel::TexInfo> m_Textures;
       std::vector<ResModel::Bone> m_Bones;
       std::vector<ResModel::Animation> m_Animations;
  };

  class ResFont : public Resource
  {
    public:
      ResFont( const Text& font )
        : Resource( RS_FONT ), m_font( font )
      {
      }

      const Text& getFont() const
        {
          return m_font;
        }

    private:
      Text m_font;

  };

  class ResTexture : public Resource
  {
    protected:
      QImage texture;
      unsigned int texId;
    public:
      const QImage& getQImage() const
      {
        return texture;
      }

      ResTexture()
        : Resource(RT_TEXTURE), texId(0)
      {}

      ResTexture(const QImage &image, const int& id )
        : Resource(RT_TEXTURE), texId( id )
      {
        texture = image;
      }

      size_t getWidth()
      {
        return texture.width();
      }

      size_t getHeight()
      {
        return texture.height();
      }

      int getTexture()
      {
        return texId;
      }
  };


  class ResAnimation : public ResTexture
  {
    public:
      ResAnimation()
        : ResTexture()
      {}

      ResAnimation
        (
        const QImage& image,
        const int& id,
        const size_t& rWidth,
        const size_t& rHeight,
        const size_t& frames
        ) : ResTexture( image, id ), m_width( rWidth ), m_height( rHeight ), m_numFrames( frames )
      {

      }

      Rect calcTexCoord( const unsigned int& frame ) const
      {
        if( frame >= m_numFrames )
        {
          THROW (Exception, "Animation Frame Out of Bounds:\nFrame: %i\nMax Frame:%lu",frame, m_numFrames - 1);
        }

        int width = (int)( texture.width()/m_width);
        int height = (int)( texture.height()/m_height);

        int i = frame / (m_width);
        int j = frame % (m_width);

        Rect tRect;
        /*tRect.bottomLeft = Coord( (float)j*width / (float)texture.width(), (float)i*height / (float)texture.height());
        tRect.upLeft = Coord( (float)j*width / (float)texture.width(), (float)(i+1)*height / (float)texture.height());
        tRect.upRight = Coord( (float)(j+1)*width / (float)texture.width(), (float)(i+1)*height / (float)texture.height());
        tRect.bottomRight = Coord( (float)(j+1)*width / (float)texture.width(), (float)i*height / (float)texture.height() );*/

        // this works with #1
        tRect.bottomLeft = Coord( (float)j*width / (float)texture.width(), (float)(i+1)*height / (float)texture.height());
        tRect.upLeft = Coord( (float)j*width / (float)texture.width(), i*height / (float)texture.height());
        tRect.upRight = Coord( (float)(j+1)*width / (float)texture.width(), (float)i*height / (float)texture.height());
        tRect.bottomRight = Coord( (float)(j+1)*width / (float)texture.width(), (float)(i+1)*height / (float)texture.height() );


        return tRect;
      }

      size_t frames() const
      {
        return m_numFrames;
      }

    private:
      size_t m_width, m_height;
      size_t m_numFrames;

  };

} // visualizer

#endif // RESOURCE_H
