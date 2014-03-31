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
#include "../resourcemanager/resource.h"
#include "../resourcemanager/resourceman.h"
#include "../common/glm/glm.hpp"
#include "../common/glm/gtc/matrix_transform.hpp"
#include "../common/glm/gtc/type_ptr.hpp"
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

    class _Renderer : public Module, public UpdateNeeded,  public IRenderer
    {
        Q_INTERFACES( IRenderer )

        friend class RenderWidget;

    public:

        _Renderer();
        ~_Renderer();

        // Module Overrides
        static void setup();

        static void destroy();

        // UpdateNeeded Overrides
        void update();

        // IRenderer Overrides

        bool init();

        bool refresh();

        bool resize
          (
           const unsigned int & width,
           const unsigned int & height,
           const unsigned int & depth = 1
          );

        bool isSetup() const {return m_isSetup;}

        bool fboSupport() const;

        bool shaderSupport() const;

        void setParent( RenderWidget *parent ) {m_parent = parent;}

        unsigned int height() const { return (m_isSetup)?m_height:0;}

        unsigned int width() const {return (m_isSetup)?m_width:0;}

        unsigned int depth() const {return (m_isSetup)?m_depth:0;}

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

        void setGridDimensions
          (
           const float& sX,
           const float& sY
          );

        // returns the size of a variable type in bytes if it exists within the
        // shading language
        unsigned int shaderTypeSize
          (
            const std::string& t
          ) const;

        unsigned int vertexAttribSize
          (
            const std::string& t
          ) const;

        GLenum vertexAttribType
          (
            const std::string& t
          )const ;

        unsigned int createShader
          (
            std::vector<std::string>& attribs,
            std::vector<ResShader::ShaderInfo>& shaders
          ) const;

        void deleteShader
          (
            const unsigned int& shader
          ) const;

        unsigned int createVertexBuffer
          (
            const char* data,
            const unsigned int numOfBytes
          ) const;

        void deleteVertexBuffer
          (
            const unsigned int& vertBuf
          ) const;

        unsigned int createIndexBuffer
          (\
            const unsigned int* data,
            const unsigned int numOfBytes
          ) const;

        void deleteIndexBuffer
          (
            const unsigned int& indexBuffer
          ) const;

        void createVertexArray
          (
            ResModel* attribs,
            const ResShader* shader
          ) const;

        void deleteVertexArray
          (
            const unsigned int& vertArray
          ) const;

        void setSelectionBox(
          const float& x,
          const float& y,
          const float& sx,
          const float& sy
          ) ;

        void checkGLError() const;

    private:
        void initFBO();

        void drawFBO( int fboNum );

        void attachFBO( int fbo );

        void push() const;

        void pop() const;

        char* loadShaderFromSource(const std::string& filename) const;

        void OutputShaderErrorMessage(unsigned int shaderID) const;

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

        unsigned int m_fbo[2];
        unsigned int m_fboTexture[2];
        unsigned int m_depthBuffer[2];

        unsigned int m_screenWidth;
        unsigned int m_screenHeight;

        // test stuff
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 proj;
        unsigned int vertexBufID;
        unsigned int vertexArrayID;
        unsigned int indexBufID;
        struct vert
        {
            float x,y,z;
            float r,g,b;
        };
        // test stuff

        RenderWidget *m_parent;
    };

    extern _Renderer *Renderer;

} // visualizer

#endif                           // RENDERER_H
