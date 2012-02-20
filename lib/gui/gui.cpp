#include "gui.h"
#include "../games/games.h"
#include "../renderer/renderer.h"
#include "../beanstalker/beanstalker.h"
#include "version.h"
#include <QDesktopServices>
#include <Qt>
#include <QFtp>
#include "optionsmanager/optionsdialog.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

namespace visualizer
{

  _GUI *GUI = 0;

  _GUI::~_GUI()
  {
  }

  bool _GUI::setup()
  {
    if( !GUI )
    {
      GUI = new _GUI;
    } 

    GUI->m_isSetup = GUI->doSetup();
    return GUI->m_isSetup;
  }


  bool _GUI::clear()
  {
    return true;
  }

  bool _GUI::create()
  {

    return true;
  }


  bool _GUI::destroy()
  {
    if( !clear() )
      return false;

    return true;
  }

  bool _GUI::isSetup()
  {
    return m_isSetup;
  }


  void _GUI::dragEnterEvent( QDragEnterEvent* evt )
  {
    // We may want to filter what's dropped on the window at some point
    evt->acceptProposedAction();
  }

  void _GUI::loadGamestring( char* log, const size_t& length, const string& gamelog )
  {
    bool parserFound = false;

    std::string fullLog;
    if( log[ 0 ] == 'B' && log[ 1 ] == 'Z' && ( log[ 2 ] == 'h' || log[ 2 ] == '0' ) )
    {
      const int megs = 25;
      unsigned int size = megs * 1024*1024;
      char *output = new char[ size-1 ];

      BZ2_bzBuffToBuffDecompress( output, &size, log, length, 0, 0 );
      output[ size ] = 0;
      fullLog = output;

      delete [] output;     
    }
    else
    {
      fullLog = log;
    }


    for
      ( 
      std::vector<IGame*>::iterator i = Games->gameList().begin(); 
      i != Games->gameList().end() && !parserFound;
      i++ 
      )
    {
      QRegExp rx( (*i)->getPluginInfo().gamelogRegexPattern.c_str() );
      rx.setPatternSyntax( QRegExp::RegExp2 );
      
      if( rx.indexIn( fullLog.c_str() ) != -1 )
      {
        TimeManager->setTurn( 0 );
        if( (*i)->getPluginInfo().returnFilename )
        {
          (*i)->loadGamelog( gamelog );
        }
        else
        {
          (*i)->loadGamelog( fullLog );
        }
        parserFound = true;
      }

    }

    if( !parserFound )
    {
      WARNING( "An appropriate game player could not be found!" );
    }
  }

  void _GUI::loadGamelog( std::string gamelog )
  {
    ifstream file_gamelog( gamelog.c_str(), ifstream::in );
    if( file_gamelog.is_open() )
    {
      
      size_t length;
      file_gamelog.seekg( 0, ios::end );
      length = file_gamelog.tellg();
      file_gamelog.seekg( 0, ios::beg );

      char *input = new char[ length + 1 ];
      input[ length ] = 0;
      file_gamelog.read( input, length );

      file_gamelog.close();

      loadGamestring( input, length, gamelog );

      delete [] input;
    }

  }


  void _GUI::update()
  {
    m_controlBar->update();
  }


  void _GUI::dropEvent( QDropEvent* evt )
  {
    const QMimeData* mimeData = evt->mimeData();
    
    if( mimeData->hasUrls() )
    {
      QStringList pathList;
      QList<QUrl> urlList = mimeData->urls();

      for( size_t i = 0; i < (unsigned int)urlList.size() && i < 32; ++i )
      {
        pathList.append( urlList.at( i ).toLocalFile() );
      }

      string path = urlList.at( 0 ).toLocalFile().toAscii().constData();
      loadGamelog( path );


    }

  }


  void _GUI::appendConsole( string line )
  {
    QString param;
    param.append( line.c_str() );
    appendConsole( param );
  }


  void _GUI::appendConsole( QString line )
  {
    _GUI::m_consoleArea->append( line );
  }


  void _GUI::clearConsole()
  {

    _GUI::m_consoleArea->clear();
  }


  void _GUI::resizeEvent( QResizeEvent* evt )
  {
    if(!m_dockWidget->isFloating())//competitor hasn't torn off our dock window
    {
      int w = width();
      int h = height();

      if( h > w )
      {
        int temp = w;
        w = h;
        h = temp;
      }

      m_dockWidget->setMinimumWidth( w - h);
      m_dockWidget->hide();
    }
    QMainWindow::resizeEvent( evt );
  }


  void _GUI::helpContents()
  {
    QDesktopServices::openUrl( QUrl( OptionsMan->getString( "helpURL" ).c_str() ) );
  }

  void _GUI::helpAbout()
  {
    //QMessageBox *about = new QMessageBox(
    QMessageBox::about( this, "About Visualizer", VERSION_STRING );

  }

  void _GUI::fileOpen()
  {
    /* The following is almost entirely a dirty hack to persistently keep the
     * most recent directory to be default for the "Open Gamelog" dialog.
     * This works WONDERS for usability.
     */
    ifstream dirinfoIN;
    dirinfoIN.open("dirinfo.txt");
    if (dirinfoIN.is_open())
    {
      string line;
      while ( dirinfoIN.good() )
      {
        getline (dirinfoIN,line);
        m_previousDirectory.clear();
        m_previousDirectory.append(QString::fromStdString(line));
      }
      dirinfoIN.close();
    }

    QFileDialog fileDialog;

    QString filename = fileDialog.getOpenFileName(
      this,
      tr( "Open Gamelog" ),
      m_previousDirectory,
      tr( "Gamelogs (*.gamelog *.glog);;All Files (*.*)") ).toAscii().constData();

    if( filename.size() > 0 )
    {
      m_previousDirectory = filename;

      ofstream dirinfoOUT;
      dirinfoOUT.open("dirinfo.txt");
      dirinfoOUT << m_previousDirectory.toStdString();
      dirinfoOUT.close();
      loadGamelog( filename.toStdString() );
    }

  }

  void _GUI::fileSpectate()
  {
    
    // Game index/game name pair of those games supporting spectate mode
    vector< pair<int, string> > spectators;

    for( size_t i = 0; i < Games->gameList().size(); i++ )
    {
      if( Games->gameList()[ i ]->getPluginInfo().spectateMode )
      {
        spectators.push_back( pair<int, string>( i, Games->gameList()[ i ]->getPluginInfo().pluginName ) );
      }

    }

    if( spectators.size() == 0 )
    {
      WARNING( "No Games Supporting Spectator Mode Found!" );
    } else if( spectators.size() == 1 )
    {
      Games->gameList()[ spectators[0].first ]->spectate( OptionsMan->getString( "SpectateServer" ) );
    } else
    {
      WARNING( "Jake Needs To Implement A Plugin Selection Dialog" );
    }


  }

  void _GUI::closeEvent( QCloseEvent* /* event */ )
  {
    Renderer->destroy();
  }

  void _GUI::displayError( const QAbstractSocket::SocketError& err )
  {
    cout << err << endl;
  }

  bool _GUI::loadInProgress() const
  {
    return m_loadInProgress;
  }

  void _GUI::loadThatShit( bool err )
  {

  }

  void _GUI::requestGamelog()
  {
    BeanStalker b( "r09mannr4.device.mst.edu", 11300 );

    b.sendCommand( "watch visualizer-requests" );

    string glogPath = b.reserve();
    b.sendCommand( string( "delete ") + b.lastJob() );
    QUrl url( glogPath.c_str() );
    
    m_loadInProgress = true;

    m_http->setHost( url.host() );
    m_http->get( url.path() );
    
  }

  void _GUI::onClose()
  {
  }

  void _GUI::updateDone( QObject* obj )
  {
    updateInfo* inf = (updateInfo*)obj;

    QFile compare( inf->version.c_str() );
    if( !compare.open( QIODevice::ReadOnly | QIODevice::Text ) )
      return;

    QByteArray data = compare.readAll();

#if 0
    cout << data.constData() << endl;
    cout << "=============" << endl;
    cout << inf->buffer->buffer().constData() << endl;
#endif

    
    if( strcmp( data.constData(), inf->buffer->buffer().constData() ) )
    {
      m_updateBar->show();
      QLabel *text = (QLabel*)m_updateBar->widget();
      text->setText( inf->message.c_str() );
    }

    compare.close();
    
    inf->buffer->close();
    
  }

  void _GUI::checkForUpdate( string message, string VERSION, string REMOTE )
  {
    QSignalMapper *map = new QSignalMapper(this);
    updateInfo* i = new updateInfo;
    i->buffer = new QBuffer(this);
    i->buffer->open( QBuffer::ReadWrite );
    i->message = message;
    i->version = VERSION;
    i->remote = REMOTE;

    QFtp *ftp = new QFtp( this );
    connect( ftp, SIGNAL(done(bool)), map, SLOT(map()) );
    map->setMapping( ftp, i );
    //connect( ftp, SIGNAL(done(bool)), this, SLOT(updateDone(bool)) );
    connect( map, SIGNAL( mapped( QObject* ) ), this, SLOT(updateDone( QObject* )) ); 

    ftp->connectToHost( "r99acm.device.mst.edu", 2121 );
    ftp->login();
    
    ftp->cd( "jenkins" );
    ftp->get( REMOTE.c_str(), i->buffer );
    ftp->close();
  }

  bool _GUI::doSetup()
  {
    checkForUpdate( "Visualizer Core", "checkList.md5", VERSION_FILE );

    m_loadInProgress = false;


    setAcceptDrops( true );

    m_centralWidget = new CentralWidget( this );
    setCentralWidget( m_centralWidget );
    createActions();
    buildControlBar();
    buildUpdateBar();

    setWindowIcon( QIcon( "icon.png" ) );

    createMenus();
    buildToolSet();

    // If we're in arenaMode, change some settings

    if( OptionsMan->getNumber( "arenaMode" ) )
    {
      menuBar()->hide();
      setFullScreen(true);
      m_dockWidget->hide();
      requestGamelog();
    }

    //If we're in demonstrationMode, change different settings
    if( OptionsMan->getNumber( "demonstrationMode" ) )
    {
      menuBar()->hide();
      setFullScreen(true);
      m_dockWidget->hide();
    }

    setWindowState(
      windowState()
      | Qt::WindowActive
      | Qt::WindowMaximized
      );

    show();

    m_previousDirectory = QDir::homePath();



    return true;
  }

  void _GUI::newReadyConnect()
  {

  }

  void _GUI::newConnect()
  {

  }

  void _GUI::buildUpdateBar()
  {
    m_updateBar = 
      new QDockWidget( "Updates Available For:", this );
    m_updateBar->setFeatures( QDockWidget::DockWidgetClosable );
    m_updateBar->setWidget( new QLabel( m_updateBar ) );
    
    addDockWidget( Qt::TopDockWidgetArea, m_updateBar );

    m_updateBar->hide();
  }

  void _GUI::buildControlBar()
  {
    m_statusBar = statusBar();
    m_statusBar -> setMaximumHeight(20);
    m_controlBar = new ControlBar( this );

    m_statusBar->addPermanentWidget( m_controlBar, 100 );

  }


  void _GUI::createActions()
  {
    m_helpContents = new QAction( tr( "&Contents" ), this );
    m_helpContents->setShortcut( tr( "F1" ) );
    m_helpContents->setStatusTip(
      tr( "Open Online Help For This Game" )
      );
    connect( m_helpContents, SIGNAL(triggered()), this, SLOT(helpContents()) );

    m_helpAbout = new QAction( tr( "&About" ), this );
    m_helpAbout->setStatusTip(
      tr( "About The Visualizer" )
      );
    connect( m_helpAbout, SIGNAL(triggered()), this, SLOT(helpAbout()) );


    m_fileOpen = new QAction( tr( "&Open" ), this );
    m_fileOpen->setShortcut( tr( "Ctrl+O" ) );
    m_fileOpen->setStatusTip(
      tr( "Open A Gamelog" )
      );
    connect( m_fileOpen, SIGNAL(triggered()), this, SLOT(fileOpen()) );

    m_fileSpectate = new QAction( tr( "&Spectate" ), this );
    m_fileSpectate->setShortcut( tr( "Ctrl+s" ) );
    m_fileSpectate->setStatusTip( tr( "Spectate or Join a Game" ) );
    connect( m_fileSpectate, SIGNAL(triggered()), this, SLOT(fileSpectate()) );

    toggleFullScreenAct = new QAction( tr("&Full Screen"), this );
    toggleFullScreenAct->setShortcut( tr("F11" ) );
    toggleFullScreenAct->setStatusTip( tr("Toggle Fullscreen Mode") );
    connect( toggleFullScreenAct, SIGNAL(triggered()), this, SLOT(toggleFullScreen()) );

    m_editOptions = new QAction( tr( "&Options" ), this );
    m_editOptions->setStatusTip( tr( "Edit Program Options" ) );
    connect( m_editOptions, SIGNAL( triggered() ), this, SLOT( optionsDialog() ) );

    m_fileExit = new QAction( tr( "&Quit" ), this );
    m_fileExit->setShortcut( tr( "Ctrl+Q" ) );
    m_fileExit->setStatusTip(
      tr( "Close the Visualizer" )
      );
    connect( m_fileExit, SIGNAL(triggered()), this, SLOT(close()) );

    (void) new QShortcut( QKeySequence( tr( "Space" ) ), this, SLOT( togglePlayPause() ) );
    (void) new QShortcut( QKeySequence( tr( "Ctrl+F" ) ), this, SLOT( fastForwardShortcut() ) );
    (void) new QShortcut( QKeySequence( tr( "Ctrl+R" ) ), this, SLOT( rewindShortcut() ) );
    (void) new QShortcut( QKeySequence( tr( "Right" ) ), this, SLOT( stepTurnForwardShortcut() ) );
    (void) new QShortcut( QKeySequence( tr( "Left" ) ), this, SLOT( stepTurnBackShortcut() ) );
    (void) new QShortcut( QKeySequence( tr("Escape") ), this, SLOT( catchEscapeKey() ) );

    //Ugly hack
    (void) new QShortcut( QKeySequence( Qt::Key_1 ), this, SLOT( turnPercentageShortcut1() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_2 ), this, SLOT( turnPercentageShortcut2() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_3 ), this, SLOT( turnPercentageShortcut3() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_4 ), this, SLOT( turnPercentageShortcut4() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_5 ), this, SLOT( turnPercentageShortcut5() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_6 ), this, SLOT( turnPercentageShortcut6() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_7 ), this, SLOT( turnPercentageShortcut7() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_8 ), this, SLOT( turnPercentageShortcut8() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_9 ), this, SLOT( turnPercentageShortcut9() ) );
    (void) new QShortcut( QKeySequence( Qt::Key_0 ), this, SLOT( turnPercentageShortcut0() ) );
  }


  void _GUI::createMenus()
  {
    QMenu *menu;
    menu = menuBar()->addMenu( tr( "&File" ) );
    menu->addAction( m_fileOpen );
    menu->addAction( m_fileSpectate );
    menu->addSeparator();
    menu->addAction( m_fileExit );

    menu = menuBar()->addMenu( tr( "&Edit" ) );
    menu->addAction( m_editOptions );

    menu = menuBar()->addMenu( tr( "&View" ) );
    menu->addAction(toggleFullScreenAct);

    menu = menuBar()->addMenu( tr( "&Help" ) );
    menu->addAction( m_helpContents );
    menu->addSeparator();
    menu->addAction( m_helpAbout );

  }


  void _GUI::buildToolSet()
  {
    // Create the dock
    m_dockWidget = new QDockWidget( this );
    // Give it a frame to hold a layout
    m_dockLayoutFrame = new QFrame( m_dockWidget );
    // Give this frame a layout
    m_dockLayout = new QVBoxLayout( m_dockLayoutFrame );
    // Console area to the left
    m_consoleArea = new QTextEdit( m_dockLayoutFrame );
    m_consoleArea -> setReadOnly(1);

    // Allow users to stupidly move this as small as they like
    m_dockWidget->setMinimumHeight( 0 );
    m_dockWidget->setMinimumWidth( 0 );

    // Add Buffer so we don't feel claustrophobic
    m_dockLayout->setContentsMargins( 2, 0, 2, 0 );

    // Add the console to the layout
    m_dockLayout->addWidget( m_consoleArea );

    // Add the frame to the actual dock
    m_dockWidget->setWidget( m_dockLayoutFrame );
    // Add the dock to the main window
    addDockWidget( Qt::RightDockWidgetArea, m_dockWidget );

  }


  void _GUI::closeGUI()
  {
    close();
  }


  void _GUI::toggleFullScreen()
  {
    setFullScreen(!fullScreen);
  }


  void _GUI::togglePlayPause()
  {
    m_controlBar->play();
  }


  void _GUI::fastForwardShortcut()
  {
    m_controlBar->fastForward();
  }


  void _GUI::rewindShortcut()
  {
    m_controlBar -> rewind();
  }


  void _GUI::turnPercentageCalc(int value)
  {
    float turnPercent = value /9.0;
    TimeManager->setTurn((int) floor(turnPercent * TimeManager->getNumTurns()));
  }


  void _GUI::stepTurnForwardShortcut()
  {
    TimeManager->setSpeed(0);
    if( TimeManager->getTurnPercent() != 1-0.99999f && TimeManager->getTurnPercent() != 0.99999f && TimeManager->getTurnPercent() != 0.99999f-0.5f )
    {
      TimeManager->setTurnPercent( 0.99999f );
    }

    TimeManager->setTurnPercent( TimeManager->getTurnPercent() + 0.5f );
  }

  void _GUI::optionsDialog()
  {
    OptionsDialog* od = new OptionsDialog;

  }

  void _GUI::stepTurnBackShortcut()
  {
    TimeManager->setSpeed(0);
    if( TimeManager->getTurnPercent() != 0 && TimeManager->getTurnPercent() != 1 && TimeManager->getTurnPercent() != 0.5f )
    {
      TimeManager->setTurnPercent( 0.0f );
    }

    TimeManager->setTurnPercent( TimeManager->getTurnPercent() - 0.5f );
  }


  //Prepares the tabs and tables for the unit stats area
  void _GUI::initUnitStats()
  {
    //TODO Move this game-specific code out of _GUI
  }


  ControlBar * _GUI::getControlBar()
  {
    return m_controlBar;
  }


  void _GUI::catchEscapeKey()
  {
    if(getFullScreen())
    {
      setFullScreen(false);
    }
  }


  bool _GUI::getFullScreen()
  {
    return fullScreen;
  }


  void _GUI::setFullScreen(bool value)
  {
    fullScreen = value;
    if(fullScreen)
    {
      m_normalWindowGeometry = geometry();
      showFullScreen();
    }
    else
    {
      showNormal();
      setGeometry(m_normalWindowGeometry);
    }
    show();

  }


  void _GUI::turnPercentageShortcut1(){turnPercentageCalc(0);};
  void _GUI::turnPercentageShortcut2(){turnPercentageCalc(1);};
  void _GUI::turnPercentageShortcut3(){turnPercentageCalc(2);};
  void _GUI::turnPercentageShortcut4(){turnPercentageCalc(3);};
  void _GUI::turnPercentageShortcut5(){turnPercentageCalc(4);};
  void _GUI::turnPercentageShortcut6(){turnPercentageCalc(5);};
  void _GUI::turnPercentageShortcut7(){turnPercentageCalc(6);};
  void _GUI::turnPercentageShortcut8(){turnPercentageCalc(7);};
  void _GUI::turnPercentageShortcut9(){turnPercentageCalc(8);};
  void _GUI::turnPercentageShortcut0(){turnPercentageCalc(9);};

  QTableWidget * _GUI::getIndividualStats()
  {
    return m_individualStats;
  }


  QTableWidget * _GUI::getSelectionStats()
  {
    return m_selectionStats;
  }


  QTableWidget * _GUI::getGlobalStats()
  {
    return m_globalStats;
  }

  const Input& _GUI::getInput() const
  {
    return m_input;
  }

  void _GUI::clearInput()
  {
    m_input.clear();
  }

} // visualizer

