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

  bool _Renderer::resize(const unsigned int & width, const unsigned int & height, const unsigned int & depth)
  {
    unsigned int _height = height?height:1;

    viewport( 0, 0, width, _height );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho( 0, width,_height,0, 0, 20 );
    cout << "DEPTH: " << depth << endl;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    m_height = _height;
    m_width = width;
    m_depth = depth;
    refresh();

    return true;
  }

  void _Renderer::viewport( int x, int y, int width, int height )
  {
    static int _x = -1;
    static int _y = -1;
    static int _width = -1;
    static int _height = -1;

    if( width != _width || height != _height || x != _x || y != _y )
    {
      glViewport( x, y, width, height );
      _width = width;
      _height = height;
      _x = x;
      _y = y;
    }
  }

  void _Renderer::checkGLError() const
  {
    GLenum errCode;
    const char* errString;

    if( (errCode = glGetError()) != GL_NO_ERROR )
    {
      WARNING( "OpenGL Error: %s", gluErrorString(errCode) );
    }
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
    glBindTexture(GL_TEXTURE_2D, fboTexture[fboNum]);

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

  }

  void _Renderer::attachFBO(int fbo)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }

  void _Renderer::swapFBO()
  {
    
    m_currentFBO = 1-m_currentFBO;
    attachFBO(fbo[m_currentFBO]);
    drawFBO(1-m_currentFBO);

  }

  bool _Renderer::refresh()
  {
    if (!isSetup())
      return false;

    if( SelectionRender->getUpdated() )
    {
      update( TimeManager->getTurn(), 0 );
    }

    if( fboSupport() )
    {
      glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if( fboSupport() )
    {
      for( size_t i = 0; i < 2; i++ )
      {
        attachFBO(fbo[i]);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      }
      m_currentFBO = 0;
      attachFBO( fbo[m_currentFBO] );
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GEQUAL);


    /// @TODO Need to clean up this code a bit.
    glPushMatrix();

    translate( width()*m_winX/m_unitSzX, height()*m_winY/m_unitSzY );

    float bigger = height()<width() ? height() : width();

    float factor;
    float fx = (m_winW-m_winX)/m_unitSzX;
    float fy = (m_winH-m_winY)/m_unitSzY;

    if( 
        width()/m_unitSzX < height()/m_unitSzY
      )
    {
      factor = width()/m_unitSzX;
    }
    else
    {
      factor = height()/m_unitSzY;
    }

    glScalef( factor*fx, factor*fy, 1 );

    AnimationEngine->draw();

    glPopMatrix();
 
    if( fboSupport() )
    {
      attachFBO(0);
      drawFBO(m_currentFBO);
    }
     
    setColor( Color( 0.75, 0, 0, 0.6 ) );

    drawProgressBar( m_selectX, m_selectY, m_selectSX-m_selectX, m_selectSY-m_selectY, 1, Color( 1, 0, 0 ) );

    if( m_parent )
    {
      m_parent->swapBuffers();
    }

    SelectionRender->setUpdated(false);

    return true;
  }


  void _Renderer::setParent( RenderWidget *parent )
  {
    #if 0
    if (!Single::isInit())
      return;                      //! @todo throw error
    #endif
    m_parent = parent;
  }


  void _Renderer::destroy()
  {
    if( !Renderer )
      return;

    if (!Renderer->clear())
      THROW( Exception, "Could Not Clear The Renderer" );

    TimeManager->removeRequest( Renderer );

    delete Renderer;
    Renderer = 0;

  }


  bool _Renderer::create()
  {
    m_parent = 0;
    m_height = 0;
    m_width  = 0;
    m_depth  = 10;

    return true;
  }


  void _Renderer::setup()
  {
    if( !Renderer )
    {
      Renderer = new _Renderer;
      SETUP( "Renderer" )
    }

  }

  void _Renderer::init()
  {

    fbo[0] = fbo[1] = 0;

    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
      WARNING( "FAILED TO INIT GLEW" );
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


    /** @todo fill this in with more setup information */
    if ( m_width && m_height && m_depth )
    {
      resize( m_width, m_height, m_depth );
    }
    else if ( m_width && m_height )
    {
      resize( m_width, m_height );
    }
    else
    {
      resize( 1, 1 );
    }

    MESSAGE( "Finished Resizing" );



    m_selectX = m_selectY
     = m_selectSX
     = m_selectSY = 0;
     
    clear();

    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glClearDepth( 0.0f );

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    MESSAGE( "Pre-Render Check" );

    refresh();

    MESSAGE( "Post-Render Check" );


    TimeManager->requestUpdate( Renderer );

    m_isSetup = true;

    m_screenWidth = 0;
    m_screenHeight = 0;
    for( size_t i = 0; i < QApplication::desktop()->screenCount(); i++ )
    {
      unsigned int width = QApplication::desktop()->screenGeometry(i).width();
      unsigned int height = QApplication::desktop()->screenGeometry(i).height();
      if( width > m_screenWidth )
        m_screenWidth = width;
      if( height > m_screenHeight )
        m_screenHeight = height;
    }

      
    if( fboSupport() )
    {
      MESSAGE( "Pre-FBO Setup" );

      // Generate the fbo's we need
      glGenFramebuffers(2, fbo);
      glGenRenderbuffers(2, depthBuffer);

      // Generate the fbo textures we need.
      glGenTextures(2, fboTexture);

      for( size_t i = 0; i < 2; i++ )
      {
        glBindTexture(GL_TEXTURE_2D, fboTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        // Attach fboTexture 0 to fbo 0
        glBindFramebuffer( GL_FRAMEBUFFER, fbo[i] );
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture[i], 0);

        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_screenWidth, m_screenHeight );

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer[i]);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
          WARNING( "FRAME BUFFER NOT WORKING" );
        }
      }

      MESSAGE( "Post-FBO Setup" );
    }

    MESSAGE( "Renderer Initialized!" );
	}

  bool _Renderer::isSetup() const
  {
    return m_isSetup;
  }


  bool _Renderer::clear()
  {
    std::map<int, renderObj*>::iterator it = m_renderConstant.begin();
    for(; it!=m_renderConstant.end(); it++ )
    {
      delete (it->second);
    }

    m_renderConstant.clear();

    return true;
  }


  bool _Renderer::registerConstantObj( const unsigned int& id, renderObj* obj )
  {
    if( m_renderConstant.find( id ) != m_renderConstant.end() )
    {
      return false;
      delete m_renderConstant[id];
    }

    m_renderConstant[id] = obj;

    return true;
  }


  bool _Renderer::deleteConstantObj( const unsigned int&/* id */ )
  {
    return false;
  }


  unsigned int _Renderer::width() const
  {
    if (isSetup())
      return m_width;

    return 0;
  }


  unsigned int _Renderer::height() const
  {
    if (isSetup())
      return m_height;

    return 0;
  }


  unsigned int _Renderer::depth() const
  {
    if (isSetup())
      return m_depth;

    return 0;
  }

  void _Renderer::update()
  {
    _Renderer::refresh();
    _Renderer::update( TimeManager->getTurn(), 0 );
  }


  bool _Renderer::update(const unsigned int & /*turn*/, const unsigned int & /*frame*/)
  {
    return true;
  }


  void _Renderer::setColor
    (
    const Color& c
    ) const
  {
    glColor4f( c.r, c.g, c.b, c.a );
  
  } // _Renderer::setColor()

  void _Renderer::setSelectionBox(
    const float& x,
    const float& y,
    const float& sx, 
    const float& sy
    ) 
  {
    m_selectX = x;
    m_selectY = y;
    m_selectSX = sx;
    m_selectSY = sy;

  } // _Renderer::setSelectionBox()

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

    glDisable( GL_BLEND );
    
  } // _Renderer::drawQuad()

  void _Renderer::drawTexturedQuad
    (
    const float& x,
    const float& y,
    const float& w,
    const float& h,
    const std::string& resource,
    const float& z
    ) const
  {
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    ResTexture *r = (ResTexture*)ResourceMan->reference( resource, "renderer" );

    glBindTexture( GL_TEXTURE_2D, r->getTexture() );
    glBegin( GL_QUADS );
      glTexCoord2f( 1, 1 ); glVertex3f( x, y, z );
      glTexCoord2f( 0, 1 ); glVertex3f( x+w, y, z );
      glTexCoord2f( 0, 0 ); glVertex3f( x+w, y+h, z );
      glTexCoord2f( 1, 0 ); glVertex3f( x, y+h, z );
    glEnd();

    ResourceMan->release( resource, "renderer" );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    
  } // _Renderer::DrawTexturedQuad()


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
    //glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    ResAnimation *r = (ResAnimation*)ResourceMan->reference( resource, "renderer" );
    Rect texCoord = r->calcTexCoord( frameNumber );

    // todo: fix this, they are reversed
    glBindTexture( GL_TEXTURE_2D, r->getTexture() );
    glBegin( GL_QUADS );
    
      /*glTexCoord2f(texCoord.upLeft.x,texCoord.upLeft.y); glVertex3f( x, y, z );
      glTexCoord2f(texCoord.bottomLeft.x,texCoord.bottomLeft.y); glVertex3f( x, y+h, z );
      glTexCoord2f(texCoord.bottomRight.x,texCoord.bottomRight.y); glVertex3f( x+w, y+h, z );   
      glTexCoord2f(texCoord.upRight.x,texCoord.upRight.y ); glVertex3f( x+w, y, z );  */
    
      // this works with #1
      glTexCoord2f(1.0f-texCoord.upLeft.x, 1.0f-texCoord.upLeft.y); glVertex3f( x, y, z );
      glTexCoord2f( 1.0f-texCoord.bottomLeft.x,1.0f-texCoord.bottomLeft.y); glVertex3f( x, y+h, z );
      glTexCoord2f( 1.0f-texCoord.bottomRight.x, 1.0f-texCoord.bottomRight.y); glVertex3f( x+w, y+h, z );   
      glTexCoord2f( 1.0f-texCoord.upRight.x, 1.0f-texCoord.upRight.y ); glVertex3f( x+w, y, z );   
    glEnd();

    ResourceMan->release( resource, "renderer" );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    
  } // _Renderer::drawAnimQuad()

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

    glDisable( GL_BLEND );
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


    glDisable( GL_BLEND );
  }


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

    glDisable( GL_BLEND );
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

  int _Renderer::createShaderProgram() const
  {
    return glCreateProgram();
  }

  void _Renderer::attachShader( const int& program, const string& name ) const
  {
    ResShader *r = (ResShader*)ResourceMan->reference( name, "renderer" );
    glAttachShader( program, r->getShader() );
    ResourceMan->release( name, "renderer" );
  }

  void _Renderer::buildShaderProgram( const int& id ) const
  {
    glLinkProgram( id );
    int status;
    glGetProgramiv( id, GL_LINK_STATUS, &status );

    if( !status )
    {
      WARNING( "Program did not link correctly." );

      int logLength;
      glGetProgramiv( id, GL_INFO_LOG_LENGTH, &logLength );

      char *log = new char[logLength+1]; 
      glGetShaderInfoLog( id, logLength, 0, log );
      MESSAGE( "Linker Log: \n %s", log );
      delete [] log;

    }

  }

  void _Renderer::useShader( const int& id ) const
  {
    glUseProgram( id );
  }

  void _Renderer::beginList( const std::string& name ) const
  {
    ResourceMan->newDisplayList( name );
    ResDisplayList* dl = (ResDisplayList*)ResourceMan->reference( name, "renderer" );
    dl->startList();
    ResourceMan->release( name, "renderer" );

  } // _Renderer::beginList()

  void _Renderer::endList( const std::string& name ) const
  {
    ResDisplayList* dl = (ResDisplayList*)ResourceMan->reference( name, "renderer" );
    dl->endList();
    ResourceMan->release( name, "renderer" );

  } // _Renderer::endList()

  void _Renderer::drawList( const std::string& name ) const
  {
    ResDisplayList* dl = (ResDisplayList*)ResourceMan->reference( name, "renderer" );
    dl->draw();
    ResourceMan->release( name, "renderer" );

  } // _Renderer::drawList()

  void _Renderer::push() const
  {
    glPushMatrix();
  } // _Renderer::push()

  void _Renderer::pop() const
  {
    glPopMatrix();
  } // _Renderer::pop()

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

} // visualizer

