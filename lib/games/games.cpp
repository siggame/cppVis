#include "games.h"
#include <iostream>
using namespace std;
#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "../MegaMinerAI-12/visualizer/mars.h"

namespace visualizer
{

  _Games *Games = 0;

  void _Games::setup()
  {
    if( !Games )
    {
      Games = new _Games;
    }

    Games->_setup();
  } // _Games::setup()

  void _Games::_setup()
  {



    //IGame *game = &theGame;
    if(m_gameList.empty())
    {
         MESSAGE( "============Setting Up Plugins=======" );

#ifdef STATIC_BUILD
         Mars* theGame = new Mars;

        theGame->gui = GUI;
        theGame->animationEngine = AnimationEngine;
        theGame->options = OptionsMan;
        theGame->renderer = Renderer;
        theGame->resourceManager = ResourceMan;
        theGame->textureLoader = TextureLoader;
        theGame->timeManager = TimeManager;
        theGame->errorLog = errorLog;

        m_gameList.push_back( theGame );
#endif // STATIC_BUILD
         MESSAGE( "============Plugins Are Initialized=======" );
    }

   // }

    
  } // _Games::_setup()

  std::vector< IGame* >& _Games::gameList()
  {
    return m_gameList;
  } // _Games::gameList()

  void _Games::destroy()
  {
    delete Games;
    Games = 0;

  } // _Games::destroy()

  _Games::~_Games()
  {
    /*for
      (
      vector< QPluginLoader* >::iterator i = m_plugins.begin();
      i != m_plugins.end();
      i++
      )
    {
      (*i)->unload();
      delete *i;
    }*/

      if(!m_gameList.empty())
      {
          delete m_gameList[0];
      }

  }

} // visualizer
