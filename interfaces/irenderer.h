#ifndef IRENDER_H
#define IRENDER_H

#include <QtPlugin>
#include "animsequence.h"
#include "models.h"

#include "../lib/gameobject/gameobject.h"


///////////////////////////////////////////////////////////////////////////////
/// @class IRenderer
/// @brief The Renderer Interface For Stuff
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn IRenderer::setGridDimensions
/// @brief 
///////////////////////////////////////////////////////////////////////////////


namespace visualizer
{

  //this is a place holder
  typedef GameObject renderObj;

  class IRenderer
  {
    public:
      enum Alignment
      {
        Left, 
        Right, 
        Center
      };

      // This function will be called to set up openGL, set up the FBO,
      // and register for updates with the time manager. Anything that
      // should be done after the constructor that could fail should
      // go here.
      virtual bool init() = 0;

      // this is the render function, should call all the drawing
      virtual bool refresh() = 0;

      // This function is responsible for resizing the viewport when the Gui
      // and in turn the render widget, is resized
      virtual bool resize
        (
        const unsigned int& width,
        const unsigned int& height,
        const unsigned int& depth
        ) = 0;

      // returns true if the setup function has been called.
      virtual bool isSetup() const = 0;

      // returns true if the level of OGL supports frame buffer objects
      virtual bool fboSupport() const= 0;

      // returns the current height of the viewport
      virtual unsigned int height() const = 0;

      // returns the current width of the viewport
      virtual unsigned int width() const = 0;

      // return the displacement of the camera in the z-axis for the fixed function
      // pipepline
      virtual unsigned int depth() const = 0;

      // Sets the color of the next primitive to be drawn
      virtual void setColor
        ( 
        const Color& c
        ) const = 0;

      // draws a two triangle/four vertex square in orthographic space relative to the
      // camera
      virtual void drawQuad
        (
        const float& x,
        const float& y,
        const float& w,
        const float& h,
        const float& z = 0.0f
        ) const = 0;

      // draws a two triangle/four vertex square in orthogorphic space relative to the
      // camera and renders a specified texture resource on it.
      virtual void drawTexturedQuad
        (
        const float& x,
        const float& y,
        const float& w, 
		const float& h,
		const float& tileFactor,
        const std::string& resource,
        const bool& flipHorizontally = false,
        const float& z = 0.0f
        ) const = 0;
        
      // draws a two triangle/four vertex square in orthographic space relative to the
      // camera and renders a specified texture resource on it. The sub coords are a
      // specific area on the texture from 0.0 to 1.0 that dictate the section of the
      // texture to be displayed.
      virtual void drawSubTexturedQuad
        (
        const float& x, 
        const float& y,
        const float& w, 
        const float& h,
        const float& subX,
        const float& subY,
        const float& subWidth,
        const float& subHeight,
        const std::string& resource,
        const float& z = 0.0f
        ) const = 0;
        
      // draws a two triangle/four vertex square in orthographic space relative to the
      // camera and renders a specified texture resource on it. The texture is rotated
      // on the quad by the number of degrees specified.
      virtual void drawRotatedTexturedQuad
        (
          const float& x,
          const float& y,
          const float& w, 
          const float& h,
		  const float& tileFactor,
          const float& degrees,
          const std::string& resource,
          const float& z = 0.0f
        ) const = 0;

      // draws a two triangle/four vertex square in orthographic space relative to the
      // camera and renders a specified animation resource on it. The animation
      // has any number of frames, and you can choose which frame to draw
      virtual void drawAnimQuad
        (
        const float& x, 
        const float& y,
        const float& w, 
        const float& h,
        const std::string& resource,
        const int& frameNumber = 0,
        const float& z = 0.0f
        ) const = 0;

      // draws a quad, that starts at x and ends at some percentage of w  plus x
      // in the orthographic view
      virtual void drawProgressBar
        (
        const float& x,
        const float& y,
        const float& w,
        const float& h,
        const float& percent,
        const Color& color = Color( 0, 0, 0, 1 ),
        const float& lineWidth = 1,
        const float& z = 0.0f
        ) const = 0;

      // draws a quad and renders a quad with some text on it in the Orthographic view
      virtual void drawText
        (
        const float& x,
        const float& y,
        const std::string& fontName,
        const std::string& line, 
        const float& size = 15.0f, 
        const Alignment& a = Left
        ) const = 0;

      // gets the total width in pixels of the string of characters in parameter line.
      virtual float textWidth
        (
         const std::string& fontName,
         const std::string& line,
         const float& size = 15.0f
        ) const = 0;

      // draws a line from two points in the Orthographic view
      virtual void drawLine
        (
        const float& sX,
        const float& sY,
        const float& eX,
        const float& eY,
        const float& width = 1.0f
        ) const = 0;
        
      // draws a curved line from two points in the Orthographic view
      virtual void drawArc
        (
         const float& centerX,
         const float& centerY,
         const float& radius,
         const int&   segments = 10,
         const float& startAngle = 0, 
         const float& endAngle = 360,
         const float& lineWidth = 1.0f,
         const Color& fillColor = Color( 0, 0, 0, 1 )
        ) const = 0;

      // translates the current set of coordinates by the x y and z passed in.
      // no change if all parameters are 0.
      virtual void translate
        (
        const float& x,
        const float& y,
        const float& z = 0.0f
        ) const = 0;

      // scales the current set of coordinates by the factor of the values
      // passed in.
      virtual void scale
        (
        const float& x,
        const float& y,
        const float& z = 1.0f
        ) const = 0;

      // rotates the current set of coordinates by the angle given (in degrees)
      // and about the vector provided.
      virtual void rotate
        (
        const float& amount,
        const float& x,
        const float& y,
        const float& z
        ) const = 0;

      // sets the x and y of the camera only. In the 2d implementation, the depth
      // is always set 10 into the z, so this will translate it in the plane
      // x, y, 10
      virtual void setCamera
        (
        const float& sX,
        const float& sY,
        const float& eX,
        const float& eY
        ) = 0;

      // sets the x and y for draw operations to that of where the grid is located
      virtual void setGridDimensions
        (
        const float& sX,
        const float& sY
        ) = 0;

      // returns the size of a variable type in bytes if it exists within the
      // shading language
      virtual unsigned int shaderTypeSize
        (
        const std::string& t
        ) const = 0;

      // returns the size of a vertex attribute if it is allowed. (things like
      // pos (position), color, norm (normal), tex(tex) )
      virtual unsigned int vertexAttribSize
        (
        const std::string& t
        ) const = 0;

  };

} // visualizer

Q_DECLARE_INTERFACE( visualizer::IRenderer, "siggame.vis2.renderer/0.2" )

#endif
