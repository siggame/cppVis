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
    MESSAGE( "============Initializing the Renderer=======" );
    setFocusPolicy(Qt::ClickFocus);
    Renderer->setup();
    glInit();
    Renderer->init();
    MESSAGE( "============Renderer Initialized=======" );
  }

  RenderWidget::~RenderWidget()
  {
  }

  void RenderWidget::initializeGL()
  {
    MESSAGE( "============Initializing OpenGL=======" );
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
  }

  void RenderWidget::mousePressEvent( QMouseEvent *e )
  {
    if( e->button() == Qt::LeftButton )
    {
      //QString line;

      x = GUI->m_input.x = e->x();
      y = GUI->m_input.y = e->y();

      adjustInput( GUI->m_input.x, GUI->m_input.y );

      leftButtonDown = true;

    }

  }

  void RenderWidget::mouseReleaseEvent( QMouseEvent *e )
  {

    //+1 guarantees we create a box, rather than a point.
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

  }
  
  void RenderWidget::wheelEvent(QWheelEvent* event)
  {
    event->accept();
    IGame* currentGame = AnimationEngine->GetCurrentGame();
    currentGame->wheelEvent(event->delta());
    
    QGLWidget::wheelEvent(event);
  }

  void RenderWidget::keyPressEvent(QKeyEvent * event)
  {
      IGame* currentGame = AnimationEngine->GetCurrentGame();
      std::string s;
      std::cout << "wakka" << std::endl;
      if (event->key() == Qt::Key_W)
        s = "W";
      else if(event->key() == Qt::Key_A)
        s = "A";
      else if(event->key() == Qt::Key_S)
        s = "S";
      else if(event->key() == Qt::Key_D)
        s = "D";
      currentGame->keyPressEvent(s);
    
      QGLWidget::keyPressEvent(event);
  }
  
} // visualizer
