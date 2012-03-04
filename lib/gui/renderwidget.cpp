#include "renderwidget.h"
#include "../renderer/renderer.h"
#include "gui.h"

#include <iostream>
using namespace std;

namespace visualizer
{

  RenderWidget::RenderWidget( QWidget *parent )
    : QGLWidget( QGLFormat( QGL::SampleBuffers ), parent )
  {
    Renderer->setup();
    glInit();
    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
      WARNING( "FAILED TO INIT GLEW" );
    }

    MESSAGE( "" );
    MESSAGE( "============System OpenGL Information=======" );
    MESSAGE( "Shader Support: %d", Renderer->shaderSupport() );
    MESSAGE( "FBO Support: %d", Renderer->fboSupport() );
    MESSAGE( "OpenGL Vendor: %s", glGetString(GL_VENDOR) );
    MESSAGE( "OpenGL Renderer: %s", glGetString(GL_RENDERER) );
    MESSAGE( "OpenGL Version: %s", glGetString(GL_VERSION) );
    MESSAGE( "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION) );
    MESSAGE( "Extensions Supported: " );
    cerr << glGetString(GL_EXTENSIONS) << endl;
    errorLog->write( (char*)glGetString(GL_EXTENSIONS) );
    MESSAGE( "============System OpenGL Information=======" );

  }

  RenderWidget::~RenderWidget()
  {
  }

  void RenderWidget::initializeGL()
  {
    Renderer->create();
    Renderer->setParent( this );
    QGLWidget::initializeGL();
  }

  void RenderWidget::resizeEvent( QResizeEvent *evt )
  {
    Renderer->resize( evt->size().width(), evt->size().height() );
  }

  void RenderWidget::adjustInput( float& x, float& y )
  {
    //float min = Renderer->width() < Renderer->height() ? Renderer->width():Renderer->height();

    float fx = (Renderer->m_winW-Renderer->m_winX)/Renderer->m_unitSzX;
    float fy = (Renderer->m_winH-Renderer->m_winY)/Renderer->m_unitSzY;
    float factor = 1;

    if( 
        width()/Renderer->m_unitSzX < height()/Renderer->m_unitSzY
      )
    {
      factor = width()/Renderer->m_unitSzX;
    }
    else
    {
      factor = height()/Renderer->m_unitSzY;
    }

    x /= factor*fx;
    y /= factor*fy;

    //x *= fx*factor*Renderer->m_unitSzX;
    //y *= fx*factor*Renderer->m_unitSzY;

#if 0
    x /= min;
    y /= min;

    x *= Renderer->m_unitSzX;
    y *= Renderer->m_unitSzY;
#endif

  }

  void RenderWidget::mousePressEvent( QMouseEvent *e )
  {
    if( e->button() == Qt::LeftButton )
    {
      //QString line;

      x = GUI->m_input.x = e->x();
      y = GUI->m_input.y = e->y();

      adjustInput( GUI->m_input.x, GUI->m_input.y );

      //+1 guarantees we create a box, rather than a point.

      //line.clear();
      //line.append("Left click: ( ");
      //line.append(QString::number(initialX));
      //line.append(", ");
      //line.append(QString::number(initialY));
      //line.append(")");

      leftButtonDown = true;

      /* Thus, dragX and dragY become our starting point,
       * and curX and curY will be contiuously updated, eventually becoming
       * our ending point if dragging.
       */
    }

  }

  void RenderWidget::mouseReleaseEvent( QMouseEvent *e )
  {

    GUI->m_input.sx = e->x()+1;
    GUI->m_input.sy = e->y()+1;

    Renderer->setSelectionBox( 0, 0, 0, 0 );

    adjustInput( GUI->m_input.sx, GUI->m_input.sy );

    if( e->button() == Qt::LeftButton )
    {
      GUI->m_input.leftRelease = true;
    } else
    {
      GUI->m_input.rightRelease = true;
    }

    //+1 guarantees we create a box, rather than a point.

    SelectionRender->setUpdated(true);
    SelectionRender->setDragging(false);

    Renderer->update( TimeManager->getTurn(), 0 );

  }

  void RenderWidget::mouseMoveEvent( QMouseEvent *e )
  {
    GUI->m_input.sx = e->x();
    GUI->m_input.sy = e->y();

    Renderer->setSelectionBox( 
      x, 
      y, 
      GUI->m_input.sx, 
      GUI->m_input.sy
      );

    adjustInput( GUI->m_input.sx, GUI->m_input.sy );

#if 0
    // If Manhattan distance is m_DRAG_DISTANCE or greater, we're draggin
    if( e->buttons() & Qt::LeftButton &&
        abs(currentX-initialX)+abs(currentY-initialY) > m_DRAG_DISTANCE )
    {
      leftButtonDrag = true;
      SelectionRender->setDragging(true);
      SelectionRender->setSelectionBox(initialX, initialY, currentX, currentY);
    }
#endif
  }

} // visualizer
