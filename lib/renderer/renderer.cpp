#define GLM_FORCE_RADIANS

#include "renderer.h"
#include "../selectionrender/selectionrender.h"
#include <sstream>
#include <math.h>
#include "gui/gui.h"

using namespace std;

namespace visualizer
{
    const float PI = 3.141592654f;

    _Renderer *Renderer = 0;

    _Renderer::_Renderer() :
        m_parent(nullptr),
        m_isSetup(false),
        m_height(0),
        m_width(0),
        m_depth(10),
        m_unitSzX(1),
        m_unitSzY(1),
        m_winX(0),
        m_winY(0),
        m_winW(30),
        m_winH(30),
        m_selectX(0),
        m_selectY(0),
        m_selectSX(0),
        m_selectSY(0),
        m_screenWidth(0),
        m_screenHeight(0)
    {
        for(int i = 0; i < 2; i++)
        {
            m_fbo[i] = 0;
            m_fboTexture[i] = 0;
            m_depthBuffer[i] = 0;
        }
    }

    _Renderer::~_Renderer()
    {
        // test stuff
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vertexBufID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &indexBufID);
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vertexArrayID);
        // test stuff
    }

    void _Renderer::setup()
    {
        if( !Renderer )
        {
            Renderer = new _Renderer;
            SETUP( "Renderer" )
        }
    }

    void _Renderer::destroy()
    {
        if( !Renderer )
            return;

        TimeManager->removeRequest( Renderer );

        delete Renderer;
        Renderer = 0;
    }

    void _Renderer::update()
    {
        this->refresh();
    }

    bool _Renderer::init()
    {
        GLenum err = glewInit();
        if( GLEW_OK != err )
        {
            WARNING( "FAILED TO INIT GLEW" );
        }

        if(!GLEW_VERSION_3_0)
        {
            WARNING( "OpenGL 3.0 not available. This program requires a minimum of OpenGL 3.0.");
        }

        int maxT;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxT);
        MESSAGE( "============System OpenGL Information=======" );
        MESSAGE( "Shader Support: %d", shaderSupport() );
        MESSAGE( "FBO Support: %d", fboSupport() );
        MESSAGE( "OpenGL Vendor: %s", glGetString(GL_VENDOR) );
        MESSAGE( "OpenGL Renderer: %s", glGetString(GL_RENDERER) );
        MESSAGE( "OpenGL Version: %s", glGetString(GL_VERSION) );
        MESSAGE( "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION) );
        MESSAGE( "Max GLSL Textures: %i", maxT );
        MESSAGE( "Extensions Supported: " );
        cerr << glGetString(GL_EXTENSIONS) << endl;
        errorLog->write( (char*)glGetString(GL_EXTENSIONS) );
        MESSAGE( "============System OpenGL Information=======" );

        glShadeModel( GL_SMOOTH );
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glClearDepth( 1.0f );

        glFrontFace(GL_CW);

        glDisable( GL_TEXTURE_2D );

        if(QApplication::desktop()->screenCount() != 0)
        {
            m_screenWidth = QApplication::desktop()->screenGeometry(0).width();
            m_screenHeight = QApplication::desktop()->screenGeometry(0).height();
        }
        else
        {
            // to avoid divide by zero errors in the case that something is VERY wrong.
            // should never reach here.
            m_screenWidth = m_screenHeight = 1;
        }

        /*
        if( fboSupport() )
        {
            initFBO();
        }
        */

        // test stuff
        float fieldOfView = PI/ 4.0f;
        float screenNear = 0.1f;
        float screenDepth = 1000.f;

        world = glm::mat4(1.0f);
        view = glm::lookAt(glm::vec3(0, 0, -20), glm::vec3(0, 0, 0), glm::vec3(0,1,0));
        proj = glm::perspectiveFov(fieldOfView, (float)m_screenWidth, (float)m_screenHeight, screenNear, screenDepth);

        // 0  1  2  3
        // 4  5  6  7
        // 8  9  10 11
        // 12 13 14 15

        vert vertices[3];
        unsigned int indices[3];
        vertices[0].x = -1.0f;
        vertices[0].y = -1.0f;
        vertices[0].z = 0.0f;

        vertices[0].r = 0.0f;
        vertices[0].g = 1.0f;
        vertices[0].b = 0.0f;

        vertices[1].x = 0.0f;
        vertices[1].y = 1.0f;
        vertices[1].z = 0.0f;

        vertices[1].r = 0.0f;
        vertices[1].g = 1.0f;
        vertices[1].b = 0.0f;

        vertices[2].x = 1.0f;
        vertices[2].y = -1.0f;
        vertices[2].z = 0.0f;

        vertices[2].r = 0.0f;
        vertices[2].g = 1.0f;
        vertices[2].b = 0.0f;

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);
        glGenBuffers(1, &vertexBufID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vert), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vert), 0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vert), (unsigned char*)NULL + (3 * sizeof(float)));
        glGenBuffers(1, &indexBufID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), &indices, GL_STATIC_DRAW);
        // test stuff


        TimeManager->requestUpdate( Renderer );

        m_isSetup = true;

        MESSAGE( "Renderer Initialized!" );
        return true;
    }

    bool _Renderer::refresh()
    {
        if (!isSetup())
            return false;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*
        if( fboSupport() )
        {
            for( size_t i = 0; i < 2; i++ )
            {
                attachFBO(m_fbo[i]);
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            }
            m_currentFBO = 0;
            attachFBO( m_fbo[m_currentFBO] );
        }
        */

        /// @TODO Need to clean up this code a bit.
        glPushMatrix();

        translate( width()*m_winX/m_unitSzX, height()*m_winY/m_unitSzY );

        float factor;
        float fx = (m_winW-m_winX)/m_unitSzX;
        float fy = (m_winH-m_winY)/m_unitSzY;

        if(width()/m_unitSzX < height()/m_unitSzY)
        {
            factor = width()/m_unitSzX;
        }
        else
        {
            factor = height()/m_unitSzY;
        }

        glScalef( factor*fx, factor*fy, 1 );

        //AnimationEngine->draw();

        glPopMatrix();

        setColor( Color( 0.75, 0, 0, 0.6 ) );

        drawProgressBar( m_selectX, m_selectY, m_selectSX-m_selectX, m_selectSY-m_selectY, 1, Color( 1, 0, 0 ) );

        // test stuff
        if(ResourceMan->exists("flat") && ResourceMan->exists("cube"))
        {
            unsigned int location;
            glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
            ResShader* s = (ResShader*)ResourceMan->reference( "flat", "renderer" );
            ResModel * m = (ResModel*)ResourceMan->reference("cube", "renderer")

            if(s->getShader() != 0)
            {
                glUseProgram(s->getShader());
                glDisable(GL_BLEND);

                checkGLError();

                static bool beenHere = false;
                if(beenHere == false)
                {
                    std::cout << "shader program ID:" << s->getShader() << std::endl;
                }

                float data[16];

                location = glGetUniformLocation(s->getShader(), "worldMatrix");
                glUniformMatrix4fv(location, 1, false, glm::value_ptr(world));

                location = glGetUniformLocation(s->getShader(), "viewMatrix");
                glUniformMatrix4fv(location, 1, false, glm::value_ptr(view));

                location = glGetUniformLocation(s->getShader(), "projectionMatrix");
                glUniformMatrix4fv(location, 1, false, glm::value_ptr(proj));

                location = glGetUniformLocation(s->getShader(), "flatColor");
                glUniform4fv(location, 1, glm::value_ptr(color));

                unsigned int vao = m->getVertexArray(s->getName());

                if(vao >= 1)
                {
                    glBindVertexArray(vao);
                    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
                }

                glUseProgram(0);
                glEnable( GL_BLEND );
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                if(beenHere == false)
                    beenHere = true;
            }

            ResourceMan->release("color", "renderer");
        }
        // test stuff

        /*
        if( fboSupport() )
        {
            attachFBO(0);
            drawFBO(m_currentFBO);
        }
        */

        if( m_parent )
        {
            m_parent->swapBuffers();
        }

        SelectionRender->setUpdated(false);

        return true;
    }

    bool _Renderer::resize(const unsigned int & width, const unsigned int & height, const unsigned int & depth)
    {
        unsigned int _height = height?height:1;

        if(width != m_width || height != m_height)
            glViewport( 0, 0, width, _height );

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho( 0, width,_height,0, 0, 20 );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        m_height = _height;
        m_width = width;
        m_depth = depth;

        glm::perspectiveFov(PI/4.0f, (float) width, (float) height, 0.1f, 1000.0f);

        refresh();

        return true;
    }

    bool _Renderer::shaderSupport() const
    {
        return QGLShader::hasOpenGLShaders( QGLShader::Vertex )
        && QGLShader::hasOpenGLShaders( QGLShader::Fragment );
    } // _Renderer::shaderSupport()

    bool _Renderer::fboSupport() const
    {
        if( string((char*)glGetString(GL_EXTENSIONS)).find( "GL_EXT_framebuffer_object" ) != -1 && OptionsMan->getNumber("Use FBO") )
        {
            return true;
        }
        return false;
    }

    void _Renderer::setColor
        (
          const Color& c
        ) const
    {
        glColor4f( c.r, c.g, c.b, c.a );
    } // _Renderer::setColor()

    void _Renderer::drawQuad
        (
          const float& x,
          const float& y,
          const float& w,
          const float& h,
          const float& z
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glBegin( GL_QUADS );
        glVertex3f( x, y, z );
        glVertex3f( x+w, y, z );
        glVertex3f( x+w, y+h, z );
        glVertex3f( x, y+h, z );
        glEnd();

        glDisable(GL_BLEND);
    } // _Renderer::drawQuad()

    void _Renderer::drawTexturedQuad
        (
          const float& x,
          const float& y,
          const float& w,
          const float& h,
          const float& tileFactor,
          const std::string& resource,
          const bool& flipHorizontally,
          const float& z
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_TEXTURE_2D );

        int U = (int)(!flipHorizontally);

        ResTexture *r = (ResTexture*)ResourceMan->reference( resource, "renderer" );

        glBindTexture( GL_TEXTURE_2D, r->getTexture() );
        glBegin( GL_QUADS );
            glTexCoord2f( tileFactor*U, tileFactor ); glVertex3f( x, y, z );          // top left
            glTexCoord2f( tileFactor*(1 - U), tileFactor ); glVertex3f( x+w, y, z );  // top right
            glTexCoord2f( tileFactor*(1 - U), 0 ); glVertex3f( x+w, y+h, z );         // bottom right
            glTexCoord2f( tileFactor*U, 0 ); glVertex3f( x, y+h, z );                 // top left
        glEnd();

        ResourceMan->release( resource, "renderer" );
        glDisable( GL_TEXTURE_2D );
        glDisable(GL_BLEND);
    } // _Renderer::DrawTexturedQuad()


    void _Renderer::drawSubTexturedQuad
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
          const float& z
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_TEXTURE_2D );

        ResTexture *r = (ResTexture*)ResourceMan->reference( resource, "renderer" );

        glBindTexture( GL_TEXTURE_2D, r->getTexture() );
        glBegin( GL_QUADS );
            glTexCoord2f( subX+subWidth,  subY+subHeight );  glVertex3f( x, y, z );
            glTexCoord2f( subX,           subY+subHeight );  glVertex3f( x+w, y, z );
            glTexCoord2f( subX,           subY );            glVertex3f( x+w, y+h, z );
            glTexCoord2f( subX+subWidth,  subY );            glVertex3f( x, y+h, z );
        glEnd();

        ResourceMan->release( resource, "renderer" );
        glDisable( GL_TEXTURE_2D );
        glDisable(GL_BLEND);
    } // _Renderer::drawSubTexturedQuad()


    void _Renderer::drawRotatedTexturedQuad
        (
          const float& x,
          const float& y,
          const float& w,
          const float& h,
          const float& tileFactor,
          const float& degrees,
          const std::string& resource,
          const float& z
        ) const
    {
        push();
        translate( x + w/2.0f, y + h/2.0f );
        rotate( degrees, 0, 0, 1 );
        drawTexturedQuad( -1 * w/2.0f, -1 * h/2.0f, w, h, tileFactor, resource );
        pop();
    } // drawRotatedTexturedQuad


    void _Renderer::drawAnimQuad
        (
          const float& x,
          const float& y,
          const float& w,
          const float& h,
          const std::string& resource,
          const int& frameNumber,
          const float& z
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_TEXTURE_2D );

        ResAnimation *r = (ResAnimation*)ResourceMan->reference( resource, "renderer" );
        Rect texCoord = r->calcTexCoord( frameNumber );

        // todo: fix this, they are reversed
        glBindTexture( GL_TEXTURE_2D, r->getTexture() );
        glBegin( GL_QUADS );
        // this works with #1
            glTexCoord2f(1.0f - texCoord.upLeft.x, 1.0f -texCoord.upLeft.y); glVertex3f( x, y, z );
            glTexCoord2f(1.0f -texCoord.bottomLeft.x,1.0f -texCoord.bottomLeft.y); glVertex3f( x, y+h, z );
            glTexCoord2f( 1.0f -texCoord.bottomRight.x,1.0f - texCoord.bottomRight.y); glVertex3f( x+w, y+h, z );
            glTexCoord2f(1.0f - texCoord.upRight.x,1.0f - texCoord.upRight.y ); glVertex3f( x+w, y, z );
        glEnd();

        ResourceMan->release( resource, "renderer" );
        glDisable( GL_TEXTURE_2D );
        glDisable(GL_BLEND);
    } // _Renderer::drawAnimQuad()

    void _Renderer::drawProgressBar
        (
          const float& x,
          const float& y,
          const float& w,
          const float& h,
          const float& percent,
          const Color& color,
          const float& lineWidth,
          const float& z
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        // The percent bar
        glBegin( GL_QUADS );
            glVertex3f( x, y, z );
            glVertex3f( x+w*percent, y, z );
            glVertex3f( x+w*percent, y+h, z );
            glVertex3f( x, y+h, z );
        glEnd();

        glColor4f( color.r, color.g, color.b, color.a );
        glLineWidth( lineWidth );

        glBegin( GL_LINE_STRIP );
            glVertex3f( x, y, z );
            glVertex3f( x+w, y, z );
            glVertex3f( x+w, y+h, z );
            glVertex3f( x, y+h, z );
            glVertex3f( x, y, z );
        glEnd();

        glDisable(GL_BLEND);
    } // _Renderer::DrawProgressBar()

    void _Renderer::drawText
        (
          const float& x,
          const float& y,
          const std::string& fontName,
          const std::string& line,
          const float& size,
          const Alignment& a
        ) const
    {
        const Text& t = ((ResFont*)ResourceMan->reference( fontName, "renderer" ))->getFont();

        glPushMatrix();
        // Scale to camera stuff.
        // Then scale proportionally to size
        glTranslatef( x, y, 0 );
        scale( size/3.0f, size/3.0f );

        switch( a )
        {
            case Left:
                t.drawLeft( line );
                break;
            case Right:
                t.drawRight( line );
                break;
            case Center:
                t.drawCenter( line );
                break;
        }

        glPopMatrix();

        ResourceMan->release( fontName, "renderer" );
    } // _Renderer::drawText()

    float _Renderer::textWidth
        (
          const std::string& fontName,
          const std::string& line,
          const float& size
        ) const
    {
        const Text& t = ((ResFont*)ResourceMan->reference( fontName, "renderer" ))->getFont();
        float width = t.getLineWidth( line );
        ResourceMan->release( fontName, "renderer" );
        return width;
    }

    void _Renderer::drawLine
        (
          const float& sX,
          const float& sY,
          const float& eX,
          const float& eY,
          const float& width
        ) const
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glLineWidth( width );

        glBegin( GL_LINES );
            glVertex2f( sX, sY );
            glVertex2f( eX, eY );
        glEnd();

        glDisable(GL_BLEND);
    }

    void _Renderer::drawArc
        (
         const float& centerX,
          const float& centerY,
          const float& radius,
          const int&   segments,
          const float& startAngle,
          const float& endAngle,
          const float& lineWidth,
          const Color& fillColor
        ) const
    {

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glLineWidth( lineWidth );
        const float convert = PI / 180.0f;
        const float interval = (endAngle-startAngle)/segments;

        glBegin( GL_LINE_STRIP);
        for( size_t i = 0; i < segments+1; i++ )
        {
            float angle = startAngle + interval*i;
            angle *= convert;
            float x = cos(angle)*radius + centerX;
            float y = sin(angle)*radius + centerY;
            glVertex2f( x, y );
        }
        glEnd();

        glDisable(GL_BLEND);
    }

    void _Renderer::translate
        (
          const float& x,
          const float& y,
          const float& z
        ) const
    {
        if( x != 0 || y != 0 || z != 0 )
            glTranslatef( x, y, z );
    } // _Renderer::translate()

    void _Renderer::scale
        (
          const float& x,
          const float& y,
          const float& z
        ) const
    {
        glScalef( x, y, z );
    } // _Renderer::scale()

    void _Renderer::rotate
        (
          const float& amount,
          const float& x,
          const float& y,
          const float& z
        ) const
    {
        glRotatef( amount, x, y, z );
    }

    void _Renderer::setCamera
        (
          const float& sX,
          const float& sY,
          const float& eX,
          const float& eY
        )
    {
        m_winX = sX;
        m_winY = sY;
        m_winW = eX;
        m_winH = eY;

        resize( width(), height() );
    } // _Renderer::setCamera()

    void _Renderer::setGridDimensions
        (
          const float& sX,
          const float& sY
        )
    {
        m_unitSzX = sX;
        m_unitSzY = sY;

        refresh();
    } // _Renderer::setUnitSize()

    void _Renderer::setSelectionBox(
        const float& x,
        const float& y,
        const float& sx,
        const float& sy)
    {
        m_selectX = x;
        m_selectY = y;
        m_selectSX = sx;
        m_selectSY = sy;
    } // _Renderer::setSelectionBox()


    void _Renderer::checkGLError() const
    {
        GLenum errCode;
        const char* errString;

        if( (errCode = glGetError()) != GL_NO_ERROR )
        {
            WARNING( "OpenGL Error: %s", gluErrorString(errCode) );
        }
    }

    void _Renderer::initFBO()
    {
        MESSAGE( "Pre-FBO Setup" );

        // Generate the fbo's we need
        glGenFramebuffers(2, m_fbo);
        glGenRenderbuffers(2, m_depthBuffer);

        // Generate the fbo textures we need.
        glGenTextures(2, m_fboTexture);

        for( size_t i = 0; i < 2; i++ )
        {
            glBindTexture(GL_TEXTURE_2D, m_fboTexture[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            // Attach fboTexture 0 to fbo 0
            glBindFramebuffer( GL_FRAMEBUFFER, m_fbo[i] );
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture[i], 0);

            glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_screenWidth, m_screenHeight );

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer[i]);

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
              WARNING( "FRAME BUFFER NOT WORKING" );
            }
        }
        MESSAGE( "Post-FBO Setup" );
    }

    void _Renderer::drawFBO( int fboNum )
    {
        glViewport( 0, 0, m_screenWidth, m_screenHeight );

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 1, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_fboTexture[fboNum]);

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glTexCoord2f(0, 1); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 1); glVertex3f(1, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(1, 1, 0);
        glTexCoord2f(0, 0); glVertex3f(0, 1, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glViewport( 0, 0, width(), height() );
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    }

    void _Renderer::attachFBO(int fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void _Renderer::push() const
    {
        glPushMatrix();
    } // _Renderer::push()

    void _Renderer::pop() const
    {
        glPopMatrix();
    } // _Renderer::pop()

    unsigned int _Renderer::shaderTypeSize
        (
          const string &t
        ) const
    {
        unsigned int size = 0;

        if(t == "int")
            size = 4;
        else if(t == "uint")
            size = 4;
        else if(t == "float")
            size = 4;
        else if(t == "double")
            size = 8;
        else if(t =="ivec2")
            size = 8;
        else if(t == "ivec3")
            size = 12;
        else if(t == "ivec4")
            size = 16;
        else if(t == "uvec2")
            size = 8;
        else if(t == "uvec3")
            size = 12;
        else if(t == "uvec4")
            size = 16;
        else if(t == "vec2")
            size = 8;
        else if(t == "vec3")
            size = 12;
        else if(t == "vec4")
            size = 16;
        else if(t == "dvec2")
            size = 16;
        else if(t == "dvec3")
            size = 24;
        else if(t == "dvec4")
            size = 32;
        else if(t == "mat2")
            size = 16; // 4 floats
        else if(t == "mat2x3")
            size = 24; // 6 floats
        else if(t == "mat2x4")
            size = 32; // 8 floats
        else if(t == "mat3x2")
            size = 24; // 6 floats
        else if(t == "mat3")
            size = 36; // 9 floats
        else if(t == "mat3x4")
            size = 48; // 12 floats
        else if(t == "mat4x2")
            size = 32; // 8 floats
        else if(t == "mat4x3")
            size = 48; // 12 floats
        else if(t == "mat4")
            size = 64; // 16 floats

        return size;
    }

    unsigned int _Renderer::vertexAttribSize
        (
          const std::string &t
        ) const
    {
        unsigned int size = 0;

        if(t == "pos")
            size = 12; // vec3
        else if(t == "color")
            size = 16; // vec4
        else if(t == "norm")
            size = 12; // vec3
        else if(t == "tan")
            size = 12; // vec3
        else if(t == "bitan")
            size = 12; // vec3
        else if(t == "tex1")
            size = 8; // vec2
        else if(t == "tex2")
            size = 8; // vec2
        else if(t == "bone1")
            size = 4; // int
        else if(t == "bone2")
            size = 4; // int
        else if(t == "bone3")
            size = 4; // int
        else if(t == "bone4")
            size = 4; // int
        else if(t == "weight1")
            size = 4; // float
        else if(t == "weight2")
            size = 4; // float
        else if(t == "weight3")
            size = 4; // float
        else if(t == "weight4")
            size = 4;

        return size;
    }

    GLenum _Renderer::vertexAttribType(const std::string& t) const
    {
        GLenum type = GL_INVALID_ENUM;

        if(t == "pos")
            type = GL_FLOAT; // vec3
        else if(t == "color")
            type = GL_FLOAT; // vec4
        else if(t == "norm")
            type = GL_FLOAT; // vec3
        else if(t == "tan")
            type = GL_FLOAT; // vec3
        else if(t == "bitan")
            type = GL_FLOAT; // vec3
        else if(t == "tex1")
            type = GL_FLOAT; // vec2
        else if(t == "tex2")
            type = GL_FLOAT; // vec2
        else if(t == "bone1")
            type = GL_INT; // int
        else if(t == "bone2")
            type = GL_INT; // int
        else if(t == "bone3")
            type = GL_INT; // int
        else if(t == "bone4")
            type = GL_INT; // int
        else if(t == "weight1")
            type = GL_FLOAT; // float
        else if(t == "weight2")
            type = GL_FLOAT; // float
        else if(t == "weight3")
            type = GL_FLOAT; // float
        else if(t == "weight4")
            type = GL_FLOAT;

        return type;
    }

    unsigned int _Renderer::createShader
        (
          std::vector<std::string>& attribs,
          std::vector<ResShader::ShaderInfo>& shaders
        ) const
    {
        bool good = true;
        int status;
        uint shaderProgram = 0;
        uint* shaderIDs = nullptr;
        const char** shaderSources = nullptr;

        shaderIDs = new uint[shaders.size()];
        for(int i = 0; i < shaders.size(); i++)
            shaderIDs[i] = 0;

        shaderSources= new const char*[shaders.size()];
        for(int i = 0; i < shaders.size(); i++)
            shaderSources[i] = nullptr;

        for(int i = 0; i < shaders.size(); i++)
        {
            shaderSources[i] = loadShaderFromSource(shaders[i].filename);
            if(shaderSources[i] == nullptr)
                good = false;
        }

        if(good)
        {
            for(int i = 0; i < shaders.size(); i++)
            {
                if(shaders[i].type == "v")
                {
                    shaderIDs[i] = glCreateShader(GL_VERTEX_SHADER);
                    glShaderSource(shaderIDs[i], 1, &shaderSources[i], NULL);
                }
                else if(shaders[i].type == "f")
                {
                    shaderIDs[i] = glCreateShader(GL_FRAGMENT_SHADER);
                    glShaderSource(shaderIDs[i], 1, &shaderSources[i], NULL);
                }
                else
                    good = false;
            }
        }

        if(good)
        {
            for(int i = 0; i < shaders.size(); i++)
            {
                glCompileShader(shaderIDs[i]);
                glGetShaderiv(shaderIDs[i], GL_COMPILE_STATUS, &status);
                if(status != 1)
                {
                    OutputShaderErrorMessage(shaderIDs[i]);
                    good = false;
                }
            }
        }

        if(good)
        {
            shaderProgram = glCreateProgram();

            for(int i = 0; i < shaders.size(); i++)
                glAttachShader(shaderProgram, shaderIDs[i]);

            for(int i = 0; i < attribs.size(); i++)
                glBindAttribLocation(shaderProgram, i, attribs[i].c_str());

             glLinkProgram(shaderProgram);
             glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
             if(status != 1)
                good = false;
        }

        for(int i = 0; i < shaders.size(); i++)
        {
            if(shaderProgram != 0)
                glDetachShader(shaderProgram, shaderIDs[i]);

            if(shaderIDs[i] != 0)
                glDeleteShader(shaderIDs[i]);

            if(shaderSources[i] != nullptr)
                delete [] shaderSources[i];
        }

        if(shaderProgram != 0 && !good)
        {
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }

        delete [] shaderSources;
        delete shaderIDs;

        return shaderProgram;
    }

    void _Renderer::deleteShader
        (
          const unsigned int &shader
        ) const
    {
        glDeleteProgram(shader);
    }

    unsigned int _Renderer::createVertexBuffer(const char *data,const unsigned int numOfBytes) const
    {
        unsigned int vbo = 0;

        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, numOfBytes, data, GL_STATIC_DRAW);

        return vbo;
    }

    void _Renderer::deleteVertexBuffer(const unsigned int &vertBuf) const
    {
        glDeleteBuffers(1, &vertBuf);
    }

    unsigned int _Renderer::createIndexBuffer(const unsigned int *data, const unsigned int numOfBytes) const
    {
        unsigned int ibo = 0;

        glGenBuffers(1, &ibo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfBytes, data, GL_STATIC_DRAW);

        return ibo;
    }

    void _Renderer::deleteIndexBuffer ( const unsigned int &indexBuffer) const
    {
        glDeleteBuffers(1, &indexBuffer);
    }

    void _Renderer::createVertexArray
      (
        ResModel* model,
        const ResShader* shader
      ) const
    {
        const std::vector<std::string>& neededAttribs = shader->getAttribs();
        const std::vector<ResModel::Attrib>&  attribs = model->getAttribInfo();
        const std::vector<unsigned int>& vertexBuffers = model->getVertexBuffers();
        const std::vector<unsigned int>&  indexBuffers = model->getIndexBuffers();
        int index = -1;
        GLenum type = GL_INVALID_ENUM;
        unsigned int attribNumElements;
        unsigned int vao = 0;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for(int i =0; i < neededAttribs.size(); i++)
        {
            for(unsigned int i = 0; i < vertexBuffers.size(); i++)
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[i]);

                index = -1;
                for(int j = 0; j < attribs.size(); j++)
                {

                    if(attribs[j].name == neededAttribs[i])
                    {
                        index = j;
                        break;
                    }
                }

                if(index == -1)
                {
                    glDeleteVertexArrays(1, &vao);
                    model->addVertexArray(shader->getName(), 0);
                    return;
                }


                glEnableVertexAttribArray(i);

                type = vertexAttribType(attribs[index].name);
                unsigned int attribSize = vertexAttribSize(attribs[index].name);
                if(type == GL_FLOAT)
                {
                    attribNumElements = attribSize/ sizeof(float);
                    glVertexAttribPointer(i, attribNumElements, type, false,  model->getVertexSize(), (unsigned char*)NULL + attribSize);
                }
                else if(type == GL_INT)
                {
                    attribNumElements = attribSize/ sizeof(int);
                    glVertexAttribIPointer(i, attribNumElements, type, model->getVertexSize(), (unsigned char*)NULL + attribSize);
                }
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[i]);
            }
        }        

        model->addVertexArray(shader->getName(), vao);
    }

    char * _Renderer::loadShaderFromSource(const std::string& filename) const
    {
        ifstream stream;
        int filesize = 0;
        char input;
        char* buffer;

        stream.open(filename);
        if(stream.fail())
        {
            return nullptr;
        }

        stream.get(input);
        while(!stream.eof())
        {
            filesize++;
            stream.get(input);
        }

        stream.close();

        buffer = new char[filesize + 1];
        if(!buffer)
            return nullptr;

        stream.open(filename);

        stream.read(buffer, filesize);
        stream.close();

        buffer[filesize] = '\0';

        return buffer;
    }

    void _Renderer::OutputShaderErrorMessage(unsigned int shaderId) const
    {
        int logSize, i;
        char* infoLog;
        ofstream fout;

        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

        logSize++;

        infoLog = new char[logSize];
        if(!infoLog)
        {
            return;
        }

        glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

        fout.open("shader-error.txt");

        for(i=0; i<logSize; i++)
        {
            fout << infoLog[i];
        }

        fout.close();

        return;
    }
} // visualizer

