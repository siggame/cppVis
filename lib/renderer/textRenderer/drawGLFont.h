#ifndef DRAWGLFONT_H
#define DRAWGLFONT_H

#include "glew/glew.h"
#include <GL/gl.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace visualizer
{

  struct TColor
  {
    TColor( float R, float G, float B )
    {
      r = R;
      g = G;
      b = B;
      index = 0;
    }
    float r,g,b;
    int index;
  };

  enum
  {
    align_left = 0,
    align_center,
    align_right
  };

  class DrawGLFont
  {
    public:
      DrawGLFont();
      DrawGLFont( int fontTextureId, string fontWidthsFile )
      {
        loadNewFont( fontTextureId, fontWidthsFile );
      }

      void setBold( bool isBold )
      {
        bold = isBold;
      }

      bool loadNewFont( int fontTextureId, string fontWidthsFile );

      void drawString( string message );

      void setAlignment( int align );

      void setColor( int &i, string message );

      void resetColors();
      void addColor( float r, float g, float b );
      TColor retrieveColor( const unsigned int & id );

    private:
      void drawAlignedLeft( string message, int offset );
      void drawAlignedCenter( string message );
      void drawAlignedRight( string message, int offset );

    private:

      // Don't want to waste a bunch of time rebinding the font
      // So we'll make this private and do the binding in
      // drawString
      void drawCharacter( unsigned char c );

      // We know there's 256 widths
      char widths[256];

      // ID of texture to bind
      int textureId;

      float kerning;
      bool bold;
      vector<TColor> colors;

      int alignment;
  };

} // visualizer

#endif
