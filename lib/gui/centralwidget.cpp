#include "centralwidget.h"
#include "gui.h"
#include <QLabel>

namespace visualizer
{

  CentralWidget::~CentralWidget()
  {
    delete m_renderWidget;
    delete m_widgetLayout;

  }


  CentralWidget::CentralWidget( QWidget* /*parent*/ )
  {
    QGLFormat fmt( QGL::SampleBuffers | QGL::DoubleBuffer | QGL::Rgba | QGL::AlphaChannel);
    fmt.setVersion(4, 1);

    m_renderWidget = new RenderWidget( this, fmt );
    m_widgetLayout = new QVBoxLayout( this );

    m_widgetLayout->setContentsMargins( 0, 0, 0, 0 );
    m_widgetLayout->addWidget( m_renderWidget );

    setLayout( m_widgetLayout );
  }

} // visualizer
