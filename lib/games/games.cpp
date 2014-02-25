#include "games.h"
#include <iostream>
using namespace std;
#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef STATIC_BUILD
#include "../chess/visualizer/chess.h"
#endif // STATIC_BUILD

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
		MESSAGE( "============Setting Up Plugins=======" );

#ifdef STATIC_BUILD
		if(!m_gameList.empty())
			return;

		Chess* theGame = new Chess;

		theGame->gui = GUI;
		theGame->animationEngine = AnimationEngine;
		theGame->options = OptionsMan;
		theGame->renderer = Renderer;
		theGame->resourceManager = ResourceMan;
		theGame->textureLoader = TextureLoader;
		theGame->timeManager = TimeManager;
		theGame->errorLog = errorLog;

		m_gameList.push_back( theGame );
#else
		IGame *game = 0;

		QDir pluginsDir( qApp->applicationDirPath() );
		QStringList pluginFilter;

		pluginFilter << "*.dll" << "*.so" << "*.dylib";
		pluginsDir.setNameFilters(pluginFilter);

		pluginsDir.cd( "plugins" );
		foreach( QString fileName, pluginsDir.entryList( QDir::Files ) )
		{
			QPluginLoader& pluginLoader = *new QPluginLoader( pluginsDir.absoluteFilePath( fileName ) );
			m_plugins.push_back( &pluginLoader );
			//QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( fileName ) );
			QObject *plugin = pluginLoader.instance();
			if( plugin )
			{
				game = qobject_cast<IGame *>( plugin );
				if( game )
				{
					#if __DEBUG__
					cerr << "Plugin Loaded: " << qPrintable( pluginsDir.absoluteFilePath( fileName ) ) << endl;
					#endif
					m_gameList.push_back( game );
					game->gui = GUI;
					game->animationEngine = AnimationEngine;
					game->options = OptionsMan;
					game->renderer = Renderer;
					game->resourceManager = ResourceMan;
					game->textureLoader = TextureLoader;
					game->timeManager = TimeManager;
					game->errorLog = errorLog;
				}
				else
				{
				MESSAGE( "Plugin is not valid: %s", qPrintable( pluginsDir.absoluteFilePath( fileName ) ) );
				WARNING( "Plugin couldn't load. Reason: \n%s", qPrintable( pluginLoader.errorString() ) );
				}
			}
			else
			{
			MESSAGE( "Plugin could not be loaded into memory. %s", qPrintable( pluginsDir.absoluteFilePath( fileName ) ) );
			WARNING( "Plugin couldn't load. Reason: \n%s", qPrintable( pluginLoader.errorString() ) );

			}
		}
#endif // STATIC_BUILD
		 MESSAGE( "============Plugins Are Initialized=======" );
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
#ifdef STATIC_BUILD
	  if(!m_gameList.empty())
	  {
		  delete m_gameList[0];
	  }
#else
	for
	  (
	  vector< QPluginLoader* >::iterator i = m_plugins.begin();
	  i != m_plugins.end();
	  i++
	  )
	{
	  (*i)->unload();
	  delete *i;
	}
#endif // STATIC_BUILD



  }

} // visualizer
