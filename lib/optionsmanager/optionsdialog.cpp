#include "optionsdialog.h"
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSignalMapper>

namespace visualizer
{
  OptionsDialog::OptionsDialog()
    : QScrollArea( 0 )
  {
    show();
    setWindowTitle( "MegaMinerAI Visualizer Options" );
    setWidget( new QWidget( this ) );
    setWidgetResizable( true );
    setMinimumWidth( 500 );
    setMinimumHeight( 400 );
    widget()->show();

    QGridLayout *layout = new QGridLayout( widget() );
    widget()->setLayout( layout );

    QSignalMapper *mapper = new QSignalMapper( this );
    connect( mapper, SIGNAL( mapped( QWidget* ) ), this, SLOT( changeOption( QWidget* ) ) );

    size_t i = 0;
    for
      ( 
        map<string, Option >::iterator j = OptionsMan->m_options.begin(); 
        j != OptionsMan->m_options.end(); 
        j++ 
      )
    {
      layout->addWidget( new QLabel( j->second.key.c_str() ), i, 0 );
      switch( j->second.type )
      {
        case OP_INT:
        case OP_FLOAT:
          if( j->second.fMinRange != -999999 && j->second.fMaxRange != 999999 )
          {
            if( j->second.fMinRange == 0 && j->second.fMaxRange == 1 )
            {
              QCheckBox *checkbox = new QCheckBox();
              checkbox->setProperty( "key", QVariant( j->second.key.c_str() ) );
              checkbox->setProperty( "type", QVariant( "checkbox" ) );

              checkbox->setChecked( j->second.fValue );


              mapper->setMapping( checkbox, checkbox );
              connect( checkbox, SIGNAL( stateChanged( int ) ), mapper, SLOT( map() ) );
              layout->addWidget( checkbox, i, 1 );

            }
            else
            {
              QSlider *slider = new QSlider( Qt::Horizontal );
              slider->setProperty( "key", QVariant( j->second.key.c_str() ) );
              slider->setProperty( "type", QVariant( "slider" ) );
              layout->addWidget( slider, i, 1 );
              mapper->setMapping( slider, slider );
              if( j->second.type == OP_INT )
              {
                slider->setMinimum( j->second.fMinRange );
                slider->setMaximum( j->second.fMaxRange );
                slider->setValue( j->second.fValue );
              }
              else
              {
                slider->setMinimum( j->second.fMinRange * 1000 );
                slider->setMaximum( j->second.fMaxRange * 1000 );
                slider->setValue( j->second.fValue * 1000 );
              }
              connect( slider, SIGNAL( valueChanged( int ) ), mapper, SLOT( map() ) );
            }

          } 
          else
          {
            QLineEdit *lineEdit = new QLineEdit( QVariant( j->second.fValue ).toString() );
            lineEdit->setProperty( "key", QVariant( j->second.key.c_str() ) );
            lineEdit->setProperty( "type", QVariant( "lineEdit" ) );
            mapper->setMapping( lineEdit, lineEdit) ;
            connect( lineEdit, SIGNAL( textEdited( const QString& ) ), mapper, SLOT( map() ) );
            layout->addWidget( lineEdit, i, 1 );

          } break;
        case OP_STRING:
        {
          QLineEdit *lineEdit = new QLineEdit( j->second.sValue.c_str() );
          lineEdit->setProperty( "key", QVariant( j->second.key.c_str() ) );
          lineEdit->setProperty( "type", QVariant( "lineEdit" ) );
          mapper->setMapping( lineEdit, lineEdit) ;
          connect( lineEdit, SIGNAL( textEdited( const QString& ) ), mapper, SLOT( map() ) );
          layout->addWidget( lineEdit, i, 1 );
        }  break;
        case OP_COMBO:
        {
          QComboBox *comboBox = new QComboBox();
          comboBox->setProperty( "key", QVariant( j->second.key.c_str() ) );
          comboBox->setProperty( "type", QVariant( "comboBox" ) );
          mapper->setMapping( comboBox, comboBox ) ;
          layout->addWidget( comboBox, i, 1 );
          for
            ( 
            vector<string>::iterator k = j->second.sOptions.begin(); 
            k != j->second.sOptions.end();
            k++
            )
          {
            comboBox->addItem( k->c_str() );
            if( !k->compare( j->second.sValue ) )
              comboBox->setCurrentIndex( comboBox->count()-1 );
          }

          connect( comboBox, SIGNAL( currentIndexChanged( int ) ), mapper, SLOT( map() ) );

        } break;
      }
      

      i++;
    }

    const int rowHeight = 30;

    widget()->setMinimumSize( 100, i*rowHeight );
    widget()->setMaximumHeight( i*rowHeight );

  }

  void OptionsDialog::changeOption( QWidget* w )
  {
    Option& o = OptionsMan->m_options[ qPrintable( w->property( "key" ).toString() ) ];

    if( w->property( "type" ).toString() == "comboBox" )
    {
      o.sValue = qPrintable( ((QComboBox*)w)->currentText() );
    }
    else if( w->property( "type" ).toString() == "lineEdit" )
    {
      o.sValue = qPrintable( ((QLineEdit*)w)->text() );
    }
    else if( w->property( "type" ).toString() == "slider" )
    {
      if( o.type == OP_INT )
        o.fValue = ((QSlider*)w)->value();
      else
        o.fValue = ((QSlider*)w)->value()/1000;
      
    }
    else if( w->property( "type" ).toString() == "checkbox" )
    {
      o.fValue = ((QCheckBox*)w)->isChecked();
    }

    OptionsMan->saveOptions();

  }


} // visualizer
