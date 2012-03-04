#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <QImage>
#include "glew/glew.h"
#include <QtOpenGL>
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
        ) : ResTexture( image, id ), width( rWidth ), height( rHeight ), numFrames( frames )
      {}
    
      Rect calcTexCoord( const unsigned int& frame ) const
      {
        if( frame >= numFrames )
        {
          THROW
            (
            Exception,
            "Animation Frame Out of Bounds"
            );
        }

        int tileX = (int)( texture.width()/width );
        //int tileY = (int)( texture.height()/height );
        int yPos = (int)frame/tileX;
        int xPos = frame-yPos*tileX;
        Rect tRect;
        // Might have to do textureWidth - each of these
        // because of how png stores textures
        tRect.upLeft = Coord( xPos*width, yPos*height );
        tRect.upRight = Coord( (xPos+1)*width, yPos*height );
        tRect.bottomRight = Coord( (xPos+1)*width, (yPos+1)*height );
        tRect.bottomLeft = Coord( xPos*width, (yPos+1)*height );

        return tRect;
      }

      size_t frames() const
      {
        return numFrames;
      }
      
    private:
      size_t width, height;
      size_t numFrames;

  };

} // visualizer

#endif // RESOURCE_H
