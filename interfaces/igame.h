#ifndef IGAME_H
#define IGAME_H

#include "ianimationengine.h"
#include "irenderer.h"
#include "iresourceman.h"
#include "icentralwidget.h"
#include "igui.h"
#include "ioptionsman.h"
#include "itextureloader.h"
#include "itimemanager.h"
#include <QtPlugin>
#include <string>

namespace visualizer
{

  struct PluginInfo
  {

    PluginInfo()
    {
      searchLength = 0;
      gamelogRegexPattern = "";
      returnFilename = false;
      spectateMode = false;
      pluginName = "NO NAME GIVEN!";
    }

    // How many characters do we scan in the gamelog
    // before we give up trying to match
    unsigned int searchLength;

    // Gamelog regular expression pattern to check any given
    // gamelog belongs to this plugin
    std::string  gamelogRegexPattern;

    // Do we want the filename or the raw gamelog string.
    // returnFilename = true passes the visualizer the filepath
    bool         returnFilename;

    // Does this plugin support spectate mode?
    bool         spectateMode;

    // What is the human readable name of this plugin?
    std::string  pluginName;

  };

  class IGame
  {
    public:
      virtual ~IGame() {}
      virtual PluginInfo getPluginInfo() = 0;
      virtual void loadGamelog( std::string gamelog ) = 0;
      virtual void spectate(const std::string& ipAddress ) {}
      virtual void destroy() = 0;

      virtual void preDraw() {}
      virtual void postDraw() {}

      virtual list<int> getSelectedUnits()
      {
        return list<int>();
      }

      virtual list<IGUI::DebugOption> getDebugOptions()
      {
        return list<IGUI::DebugOption>();
      }

      virtual std::map<std::string, bool> getRenderTagState()
      {
        return std::map<std::string, bool>();
      }

      // private slots ...or this is where I'd put one...IF I HAD ONE!!!
      virtual void pruneSelection() {}

      virtual void optionStateChanged() {}

      IAnimationEngine *animationEngine;
      IGUI *gui;
      IOptionsMan *options;
      IRenderer *renderer;
      IResourceMan *resourceManager;
      ITextureLoader *textureLoader;
      ITimeManager *timeManager;
      Log *errorLog;

  };

} // visualizer

Q_DECLARE_INTERFACE( visualizer::IGame, "siggame.vis2.game/0.3" );


#endif
