#include "optionsdialog.h"

namespace visualizer
{
  OptionsDialog::OptionsDialog()
    : QScrollArea( 0 )
  {
    show();
    setWindowTitle( "MegaMinerAI Visualizer Options" );
    setWidget( new QWidget( this ) );
    setWidgetResizable( true );
    widget()->show();

    QGridLayout *layout = new QGridLayout( widget() );
    widget()->setLayout( layout );

    


    widget()->setMinimumSize( 100, 100 );
    widget()->setMaximumHeight( 200 );

  }


} // visualizer
