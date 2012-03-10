#include <iostream>
#include "common.h"
#include "lib/gui/gui.h"
#include "lib/optionsmanager/optionsman.h"
#include "lib/timemanager/timeManager.h"
#include "lib/resourcemanager/resourceman.h"
#include "lib/renderer/renderer.h"
#include "lib/games/games.h"
#include "lib/resourcemanager/textureloader.h"

using namespace visualizer;
namespace visualizer
{
  Log* errorLog = new Log( "visualizer.log" );
}

int main(int argc, char *argv[])
{

  ///////////////////////////////////////////////////////////////////
  // Must initialize things based on their dependency graphs
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Options Manager Depends On Mutex
  ///////////////////////////////////////////////////////////////////
  OptionsMan->setup();

  OptionsMan->loadOptionFile( "./options.xml", "core" );

  // NO MEMORY LEAKS AT THIS POINT

  ///////////////////////////////////////////////////////////////////
  // Time Manager Depends On Options Manager
  ///////////////////////////////////////////////////////////////////

  TimeManager->setup();

  ///////////////////////////////////////////////////////////////////
  // ObjectManager depends on OptionsManager 
  ///////////////////////////////////////////////////////////////////
  //ObjectManager->setup();

  ///////////////////////////////////////////////////////////////////
  // GUI Depends On This Runing, but it doens't depend on anything.
  ///////////////////////////////////////////////////////////////////
	QApplication app( argc, argv );

  ///////////////////////////////////////////////////////////////////
  // SelectionRender depends on OptionsManager 
  ///////////////////////////////////////////////////////////////////
  SelectionRender->setup();

  ///////////////////////////////////////////////////////////////////
  // Resource Manager depends on _______________________
  ///////////////////////////////////////////////////////////////////
  ResourceMan->setup();

  ///////////////////////////////////////////////////////////////////
  // Initialize Texture Loader
  ///////////////////////////////////////////////////////////////////
  TextureLoader->setup();

  ///////////////////////////////////////////////////////////////////
  // Initialize Animation Engine
  ///////////////////////////////////////////////////////////////////
  AnimationEngine->setup();

  ///////////////////////////////////////////////////////////////////
  // GUI Depends On Options Manager, Time Manager, Objectmanager,
  // SelectionRender, and QApplication running already.
  // GUI also depends on the renderer, but starts it automagically.
  ///////////////////////////////////////////////////////////////////
	GUI->setup();

  ///////////////////////////////////////////////////////////////////
  // Initalize the Games
  // This MUST be initialized last, but before loading a gamelog
  // This assigns all the pointers to the interfaces and so all
  // the the interfaces must be set up already.
  ///////////////////////////////////////////////////////////////////
  Games->setup();

	if( argc > 1 )
	{
    for( size_t i = 1; i < argc; i++ )
    {
      GUI->addToPlaylist( argv[i] );
    }
	} else
  {
    GUI->splashScreen();
  }

  TimeManager->timerStart();

	int retval = app.exec();

  MESSAGE( "Destroying Games..." );
  Games->destroy();
  MESSAGE( "Destroying Animation Engine..." );
  AnimationEngine->destroy();
  MESSAGE( "Destroying Texture Loader..." );
  TextureLoader->destroy();
  MESSAGE( "Destroying Resource Manager..." );
  ResourceMan->destroy();
  MESSAGE( "Destroying GUI..." );
  GUI->destroy();
  MESSAGE( "Destroying Renderer..." );
	Renderer->destroy();
  MESSAGE( "Destroying Time..." );
  TimeManager->destroy();
  MESSAGE( "Destroying Options Manager..." );
	OptionsMan->destroy();


  MESSAGE( "Exit Successful!" );
	return retval;

}

