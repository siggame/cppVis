#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include <list>
#include <set>
#include <QTableWidget>
#include "glew/glew.h"
#include <GL/gl.h>
#include <QGLShader>
#include <string>
#include "textRenderer/drawGLFont.h"
#include "../optionsmanager/optionsman.h"
#include "../gui/renderwidget.h"
#include "../animationengine/animationengine.h"
#include "../resourcemanager/resourceman.h"

#include "../timemanager/timeManager.h"

#include "../../interfaces/irenderer.h"
#include "common.h"

namespace visualizer
{

#define renderHeightName  "renderHeight"
#define renderWidthName   "renderWidth"
#define renderDepthName   "renderDepth"
#define renderDirsName    "renderDirections"

  class RenderWidget;
  class _Renderer : public UpdateNeeded, public Module, public IRenderer
  {
    Q_INTERFACES( IRenderer );
    public:

    _Renderer()
    {
      m_isSetup = false;
      m_unitSzX = m_unitSzY = 1;
      m_winX = m_winY = 0;

      m_winW = m_winH = 30;

      m_height = m_width = m_depth = 0;
    }

    bool registerConstantObj( const unsigned int& id, renderObj* obj );
    bool deleteConstantObj( const unsigned int& id );

    static void setup();
    static void destroy();
    void init();

    bool clear();

    bool create();

    void setParent( RenderWidget *parent );

    bool refresh();
    bool resize
      (
       const unsigned int & width,
       const unsigned int & height,
       const unsigned int & depth = 1
      );

    bool isSetup() const;

    unsigned int height() const;
    unsigned int width() const;
    unsigned int depth() const;

    bool update
      (
       const unsigned int & turn,
       const unsigned int & frame
      );

    void update();

    void viewport( int x, int y, int width, int height );

    bool shaderSupport() const;
    bool fboSupport() const;

    void setColor
      (
       const Color& c
      ) const;

    void drawQuad
      (
       const float& x,
       const float& y,
       const float& w,
       const float& h,
       const float& z = 0.0f
      ) const;

    void setSelectionBox(
      const float& x,
      const float& y,
      const float& sx,
      const float& sy
      ) ;

    void drawTexturedQuad
      (
       const float& x,
       const float& y,
       const float& w, 
       const float& h, 
	   const float& tileFactor,
       const std::string& resource,
       const bool& flipHorizontally = false,
       const float& z = 0.0f
      ) const;
    
    void drawSubTexturedQuad
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
      const float& offsetX = 0.0f,
      const float& offsetY = 0.0f,
      const float& z = 0.0f
      ) const;
    
    void drawRotatedTexturedQuad
      (
        const float& x,
        const float& y,
        const float& w, 
        const float& h,
		const float& tileFactor,
        const float& degrees,
        const std::string& resource,
        const float& z = 0.0f
      ) const;
    
    void drawAnimQuad
      (
       const float& x,
       const float& y,
       const float& w, 
       const float& h, 
       const std::string& resource, 
       const int& frameNumber = 0,
       const float& z = 0.0f
      ) const;

    void drawProgressBar
      (
       const float& x,
       const float& y, 
       const float& w, 
       const float& h,
       const float& percent,
       const Color& color = Color( 0, 0, 0, 1 ),
       const float& lineWidth = 1,
       const float& z = 0.0f
      ) const; 

    void drawText
      (
       const float& x,
       const float& y,
       const std::string& fontName,
       const std::string& line, 
       const float& size = 15.0f, 
       const Alignment& a = Left
      ) const;

    float textWidth
      (
       const std::string& fontName,
       const std::string& line,
       const float& size = 15.0f
      ) const;

    void drawLine
      (
       const float& sX,
       const float& sY,
       const float& eX,
       const float& eY,
       const float& width = 1.0f
      ) const;

    void drawArc
      (
       const float& centerX,
       const float& centerY,
       const float& radius,
       const int&   segments = 10,
       const float& startAngle = 0, 
       const float& endAngle = 360,
       const float& lineWidth = 1.0f,
       const Color& fillColor = Color( 0, 0, 0, 1 )
      ) const;

    void translate
      (
       const float& x,
       const float& y,
       const float& z = 0.0f
      ) const;

    void scale
      (
       const float& x,
       const float& y,
       const float& z = 1.0f
      ) const;

    void rotate
      (
       const float& amount,
       const float& x,
       const float& y,
       const float& z
      ) const;

    void setCamera
      (
       const float& sX,
       const float& sY,
       const float& eX,
       const float& eY
      );

    int createShaderProgram() const;
    void attachShader( const int& program, const string& name ) const;
    void buildShaderProgram( const int& id ) const;
    void useShader( const int& id ) const;
    
    void beginList( const std::string& name ) const;

    void endList( const std::string& name ) const;

    void drawList( const std::string& name ) const;

    void push() const;
    void pop() const;

    void checkGLError() const;

    void setGridDimensions
      (
       const float& sX,
       const float& sY
      );

    protected:
    private:
    unsigned int m_height;
    unsigned int m_width;
    unsigned int m_depth;

    float m_unitSzX;
    float m_unitSzY;

    float m_winX;
    float m_winY;
    float m_winW;
    float m_winH;

    float m_selectX;
    float m_selectY;
    float m_selectSX;
    float m_selectSY;

    bool m_isSetup;

    int m_currentFBO;

    unsigned int fbo[2];
    unsigned int fboTexture[2];
    unsigned int depthBuffer[2];

    void swapFBO();
    void drawFBO( int fboNum );
    void attachFBO( int fbo );

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;

    std::map<int, renderObj*> m_renderConstant;
    RenderWidget *m_parent;

    friend class RenderWidget;

  };

  extern _Renderer *Renderer;

} // visualizer

#endif                           // RENDERER_H
