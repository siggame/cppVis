#ifndef IGUI_H
#define IGUI_H

#include <QTableWidget>
#include <QString>
#include <string>
#include <QtPlugin>

namespace visualizer
{

  struct Input
  {
    Input()
    {
      clear();
    }

    void clear()
    {
      leftRelease = rightRelease = false;
    }

    float x, y;
    float sx, sy;

    bool leftRelease;
    bool rightRelease;

  };

  class IGUI
  { 
    public:

      virtual bool isSetup() = 0;

      virtual void update() = 0;
      virtual void closeGUI() = 0;

      virtual void appendConsole( std::string line ) = 0;
      virtual void appendConsole( QString line ) = 0;
      virtual void clearConsole() = 0;

      virtual bool getFullScreen() = 0;
      virtual void setFullScreen( bool ) = 0;

      virtual QTableWidget* getGlobalStats() = 0;
      virtual QTableWidget* getSelectionStats() = 0;
      virtual QTableWidget* getIndividualStats() = 0;

      virtual const Input& getInput() const = 0;
  };

} // visualizer

Q_DECLARE_INTERFACE( visualizer::IGUI, "siggame.vis2.gui/0.2" );

#endif
