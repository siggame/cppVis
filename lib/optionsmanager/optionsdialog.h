#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "optionsman.h"
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>

namespace visualizer
{
  class OptionsDialog : public QScrollArea
  {
    Q_OBJECT;
    public:
      OptionsDialog();

    private slots:
      void changeOption( QWidget* w );

    private:
    
  };

} // visualizer

#endif // OPTIONSDIALOG_H
