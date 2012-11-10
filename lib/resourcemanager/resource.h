#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <QImage>
#include "glew/glew.h"
#include <GL/gl.h>
#include "common.h"
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
      ResShader( const unsigned int& id )
        : Resource( RS_VERTSHADER ), m_shaderId(id) { }

      const unsigned int& getShader() const { return m_shaderId; }

    private:
      unsigned int m_shaderId;

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
          THROW (Exception, "Animation Frame Out of Bounds:\nFrame: %i\nMax Frame:%i",frame, m_numFrames - 1);
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
