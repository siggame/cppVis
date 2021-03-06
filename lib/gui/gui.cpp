#include "gui.h"
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

  void _GUI::loadGamestring( char* log, const size_t& length, const string& gamelog, const int& startTurn )
  {
    bool parserFound = false;

    if( m_chooseDialog )
      m_chooseDialog->hide();

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
          (*i)->destroy();
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
      MESSAGE( "Log Size: %d", fullLog.size() );
      errorLog->write( fullLog );
    }
  }

  void _GUI::addToPlaylist( const std::string& gamelog, const int& startTurn )
  {
    QFileInfo file( gamelog.c_str() );

    QListWidgetItem *item = new QListWidgetItem( file.fileName() );
    item->setData( 1, QVariant( gamelog.c_str() ) );
    item->setData( 2, QVariant( startTurn ) );

    if( m_playList->count() == 0 )
      loadGamelog( gamelog, startTurn );

    m_playList->addItem( item );


  }

  void _GUI::playNext()
  {
    m_playListItem++;
    if( m_playListItem < m_playList->count() )
    {
      playItem( m_playList->item(m_playListItem) );
    }
  }

  void _GUI::playItem( QListWidgetItem* item )
  {
    loadGamelog( std::string( item->data(1).toByteArray().constData() ), item->data(2).toInt() );
  }

  void _GUI::loadGamelog( const std::string& gamelog, const int& startTurn )
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

      loadGamestring( input, length, gamelog, startTurn );

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
      QList<QUrl> urlList = mimeData->urls();

      m_playList->clear();
      m_playListItem = 0;

      for( size_t i = 0; i < (unsigned int)urlList.size(); ++i )
      {
        string path = urlList.at( i ).toLocalFile().toAscii().constData();
        addToPlaylist( path, 0 );
      }

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
    m_consoleArea->append( line );
  }

  void _GUI::clearConsole()
  {

    m_consoleArea->clear();
  }

  void _GUI::resizeEvent( QResizeEvent* evt )
  {
    //competitor hasn't torn off our dock window
    if(!m_dockWidget->isFloating())
    {
      int w = width();
      int h = height();

      if( h > w )
      {
        int temp = w;
        w = h;
        h = temp;
      }

    }
    QMainWindow::resizeEvent( evt );
  }

  void _GUI::helpContents()
  {
    QDesktopServices::openUrl( QUrl( OptionsMan->getString( "Help URL" ).c_str() ) );
  }

  void _GUI::helpLikeUs()
  {
    QDesktopServices::openUrl( QUrl( "http://facebook.com/megaminerai/" ) );
  }

  void _GUI::helpAbout()
  {
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

    QStringList filenames = QFileDialog::getOpenFileNames(
        this,
        tr( "Open Gamelog" ),
        m_previousDirectory,
        tr( "Gamelogs (*.gamelog *.glog);;All Files (*.*)") );

    if( filenames.count() )
    {
      m_playListItem = 0;
      m_playList->clear();
      for( size_t i = 0; i < filenames.count(); i++ )
      {
        if( i == 0 )
        {
          m_previousDirectory = filenames[i];
          ofstream dirinfoOUT;
          dirinfoOUT.open("dirinfo.txt");
          dirinfoOUT << m_previousDirectory.toStdString();
          dirinfoOUT.close();
        }
        addToPlaylist( filenames[i].toStdString() );

      }
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
      Games->gameList()[ spectators[0].first ]->spectate( OptionsMan->getString( "Spectate Server" ) );
    } else
    {
      QStringList plugins;
      for(auto& i: spectators)
      {
        plugins << QString(i.second.c_str());
      }

      QString text = QInputDialog::getItem(this, "Choose A Plugin To Spectate With", "Available Plugins:", plugins, 0, false);

      int index = plugins.indexOf(text);

      Games->gameList()[ spectators[index].first ]->spectate( OptionsMan->getString("Spectate Server" ) );

    }


  }

  void _GUI::closeEvent( QCloseEvent* /* event */ )
  {
    Renderer->destroy();
  }

  void _GUI::displayError( const QAbstractSocket::SocketError& err )
  {
    WARNING( "%d", err );
  }

  bool _GUI::loadInProgress() const
  {
    return m_loadInProgress;
  }

  void _GUI::loadThatShit( bool err )
  {
    if( !err )
    {
      QByteArray arr = m_http->readAll();
      if( arr.size() == 0 )
        return;
      char *temp = new char[ arr.size() ];
      memcpy( temp, arr.constData(), arr.size() );
      loadGamestring( temp, arr.size(), "", 0 );
      m_loadInProgress = false;
      delete [] temp;
    }

  }

  void _GUI::requestGamelog()
  {
    BeanStalker b( OptionsMan->getString( "Arena Server" ), 11300 );

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
    if(
        OptionsMan->getString( "Game Mode" ).compare( "Arena" ) &&
        OptionsMan->getNumber( "Check For Updates" ) )
    {
      updateInfo* inf = (updateInfo*)obj;

      QFile compare( inf->version.c_str() );
      if( !compare.open( QIODevice::ReadOnly | QIODevice::Text ) )
      {
        MESSAGE( "Local Side MD5 Compare Sum Not Found: %s", inf->version.c_str() );
        return;
      }

      QByteArray data = compare.readAll();

      if( strcmp( data.constData(), inf->buffer->buffer().constData() ) )
      {
        m_updateBar->show();

        QLabel *text = (QLabel*)m_updateBar->widget();
        text->setText( inf->message.c_str() );


        ofstream out1( "md5.in" );
        out1  << data.constData();
        out1.close();
        ofstream out2( "md5.out" );
        out2 << inf->buffer->buffer().constData();
        out2.close();


      }

      compare.close();

      inf->buffer->close();
    }

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
    MESSAGE( "============Setting Up GUI=======" );

    m_playListItem = 0;

    m_http = new QHttp( this );
    connect( m_http, SIGNAL( done( bool) ), this, SLOT( loadThatShit(bool) ) );

    m_loadInProgress = false;

    setAcceptDrops( true );

    m_centralWidget = new CentralWidget( this );
    setCentralWidget( m_centralWidget );
    MESSAGE( "============Creating Actions=======" );
    createActions();
    MESSAGE( "============Building Controlbar=======" );
    buildControlBar();
    MESSAGE( "============Building Updatebar=======" );
    buildUpdateBar();

    setWindowIcon( QIcon( "icon.png" ) );

    MESSAGE( "============Creating Menus=======" );
    createMenus();
    MESSAGE( "============Build Toolset=======" );
    buildToolSet();

    // connect set debug options. The game will call a private function
    // which will emit the signal from that thread, to the relevant QThread
    // and it will then query the game for the info to instantiate Qt
    // checkbox widget. It's roundabout, but hey, fuck QThreads.
    QObject::connect(this, SIGNAL(proxySetDebugOptionsS(IGame*)),
                     this, SLOT(proxySetDebugOptionsP(IGame*)));

    QObject::connect(TimeManager, SIGNAL(TurnChanged()),
                     this, SLOT(updateDebugWindowSLOT()));

    QObject::connect(TimeManager, SIGNAL(maxTurnsChanged(int)),
                     m_controlBar, SLOT(maxTurnsChanged(int)));

    QObject::connect(TimeManager, SIGNAL(TurnChanged()),
                     m_controlBar, SLOT(turnChanged()));

    // If we're in arenaMode, change some settings

    if( !OptionsMan->getString( "Game Mode" ).compare( "Arena" ) )
    {
      MESSAGE( "============Arena Mode=======" );
      menuBar()->hide();
      if( OptionsMan->getNumber( "Presentation Fullscreen" ) == 1 )
      {
        setFullScreen(true);
      }
      m_dockWidget->hide();
      requestGamelog();
    } else if( !OptionsMan->getString( "Game Mode" ).compare( "Demo" ) )
    {
      MESSAGE( "============Demo Mode=======" );
      //If we're in demonstrationMode, change different settings
      menuBar()->hide();
      if( OptionsMan->getNumber( "Presentation Fullscreen" ) == 1 )
      {
        setFullScreen(true);
      }
      m_dockWidget->hide();
    }

    MESSAGE( "============Set Window State=======" );
    setWindowState(
        windowState()
        | Qt::WindowActive
        | Qt::WindowMaximized
        );

    MESSAGE( "============Show Window=======" );
    show();

    MESSAGE( "============Home Path=======" );
    m_previousDirectory = QDir::homePath();
    MESSAGE( ": %s", QDir::homePath().toStdString().c_str() );

    m_chooseDialog = 0;
    MESSAGE( "============Checking For Updates=======" );
    checkForUpdate( "Visualizer Core", "checkList.md5", VERSION_FILE );


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

    if( !OptionsMan->getString( "Game Mode" ).compare( "Arena" ) )
      m_statusBar->hide();

  }

  void _GUI::createActions()
  {
    m_helpContents = new QAction( tr( "&Contents" ), this );
    m_helpContents->setShortcut( tr( "F1" ) );
    m_helpContents->setStatusTip(
        tr( "Open Online Help For This Game" )
        );
    connect( m_helpContents, SIGNAL(triggered()), this, SLOT(helpContents()) );

    m_helpLikeUs = new QAction( tr( "&Like Us On Facebook!" ), this );
    connect( m_helpLikeUs, SIGNAL(triggered()), this, SLOT(helpLikeUs()) );

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

    showDebugWindowAct = new QAction( tr("&Show Debug Window"), this );
    showDebugWindowAct->setShortcut( tr("Ctrl+d") );
    showDebugWindowAct->setStatusTip( tr( "Show the debug window." ) );
    connect( showDebugWindowAct, SIGNAL(triggered()), this, SLOT(showDebugWindow()) );

    m_editOptions = new QAction( tr( "&Options" ), this );
    m_editOptions->setShortcut( tr("F10") );
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

    QSignalMapper* mapper = new QSignalMapper(this);


    QShortcut *cut;
    for( size_t i = 0; i < 10; i++ )
    {
      cut = new QShortcut( QKeySequence( Qt::Key_0+i ), this );
      mapper->setMapping( cut, i-1 );
      connect( cut, SIGNAL( activated() ), mapper, SLOT( map() ) );
    }

    connect( mapper, SIGNAL( mapped( int ) ), this, SLOT( turnPercentageCalc(int) ) );

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
    menu->addAction(showDebugWindowAct);

    menu = menuBar()->addMenu( tr( "&Help" ) );
    menu->addAction( m_helpContents );
    menu->addAction( m_helpLikeUs );
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

    // Add Buffer so we don't feel claustrophobic
    m_dockLayout->setContentsMargins( 2, 0, 2, 0 );

    m_debugTabs = new QTabWidget(m_dockLayoutFrame);

    // Console area to the left
    m_consoleArea = new QTextEdit( m_dockLayoutFrame );
    m_consoleArea->setReadOnly(1);

    m_playList = new QListWidget( m_dockLayoutFrame );

    connect(
        m_playList,
        SIGNAL(itemDoubleClicked(QListWidgetItem *)),
        this,
        SLOT(playItem(QListWidgetItem*))
        );

    m_debugArea= new QWidget(m_dockLayoutFrame);
	m_debugTabs->insertTab( 0, m_debugArea, "Debug Table" );
	m_debugTabs->insertTab( 1, m_consoleArea, "Console" );
    m_debugTabs->insertTab( 2, m_playList, "Playlist" );

    m_debugAreaLayout = new QVBoxLayout(m_debugArea);
    m_debugOptionBox = new QFrame(m_debugArea);
    m_debugOptionsLayout = new QVBoxLayout(m_debugOptionBox);
    m_debugOptionsLabel = new QLabel("Options", m_debugOptionBox);
    m_debugSelectionsList = new QTableWidget(m_debugArea);
    m_debugSelectionInfo = new QTableWidget(m_debugArea);

    m_debugOptionBox->setFrameStyle(QFrame::Shape::Box);
    m_debugOptionBox->show();

    m_debugOptionsLabel->show();
    m_debugOptionsLayout->addWidget(m_debugOptionsLabel);

    m_debugSelectionsList->setCellWidget( 0, 0, new QLabel( "" ) );
    m_debugSelectionsList->setColumnCount(1);
    m_debugSelectionsList->setHorizontalHeaderLabels({"Selections"});
    m_debugSelectionsList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_debugSelectionsList->setSelectionBehavior( QAbstractItemView::SelectItems );
    m_debugSelectionsList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_debugSelectionsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_debugSelectionsList->verticalHeader()->setVisible(false);
    m_debugSelectionsList->setShowGrid(true);
    m_debugSelectionsList->show();

    m_debugSelectionInfo->setCellWidget( 0, 0, new QLabel( "" ) );
    m_debugSelectionInfo->setColumnCount(2);
    m_debugSelectionInfo->setHorizontalHeaderLabels({"Parameter", "Value"});
    m_debugSelectionInfo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_debugSelectionInfo->setSelectionBehavior( QAbstractItemView::SelectItems );
    m_debugSelectionInfo->setSelectionMode( QAbstractItemView::NoSelection );
    m_debugSelectionInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_debugSelectionsList->verticalHeader()->setVisible(false);
    m_debugSelectionInfo->setShowGrid(true);
    m_debugSelectionInfo->show();

    QObject::connect(m_debugSelectionsList, SIGNAL(itemSelectionChanged()),
                     this, SLOT(updateDebugInfoTable()));

    QObject::connect(TimeManager, SIGNAL(TurnChanged()),
                     this, SLOT(updateDebugInfoTable()));

    QObject::connect(TimeManager, SIGNAL(TurnChanged()),
                     this, SLOT(frameChanged()));

    m_debugAreaLayout->addWidget(m_debugOptionBox);
    m_debugAreaLayout->addWidget(m_debugSelectionsList);
    m_debugAreaLayout->addWidget(m_debugSelectionInfo);

    m_frameSlider = new QSlider( Qt::Horizontal );
    m_frameSlider->setMinimum(1);
    m_frameSlider->setMaximum(999);
    m_frameSlider->setTracking(true);
    connect( m_frameSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)) );

    m_dockLayout->addWidget( m_debugTabs );
    m_dockLayout->addWidget( m_frameSlider );

    // Add the frame to the actual dock
    m_dockWidget->setWidget( m_dockLayoutFrame );

    // Add the dock to the main window
    addDockWidget( Qt::RightDockWidgetArea, m_dockWidget );

    if( OptionsMan->getNumber( "Show Debug Window On Start" ) )
    {
      m_dockWidget->show();
    }
    else
    {
      m_dockWidget->hide();
    }

  }

  void _GUI::proxySetDebugOptionsP(IGame * game)
  {
    m_debugOptions.clear();
    for(auto& option : game->getDebugOptions())
    {
        m_debugOptions[option.name].reset(new QCheckBox(QString(option.name.c_str()), NULL));
        m_debugOptions[option.name]->setChecked(option.initValue);
        m_debugOptions[option.name]->show();
        m_debugOptionsLayout->addWidget(m_debugOptions[option.name].get());
        QObject::connect(m_debugOptions[option.name].get(), SIGNAL(stateChanged(int)),
                         this, SLOT(debugOptionStateChanged(int)));
    }
    debugOptionStateChanged(int(0));
  }

  void _GUI::updateDebugWindowSLOT()
  {
    updateDebugWindow();
  }

  void _GUI::updateDebugWindow()
  {

    static std::list<int> pastState;

    IGame* currentGame = AnimationEngine->GetCurrentGame();

    if(currentGame != NULL)
    {
        int i = 0;

        list<int> temp = currentGame->getSelectedUnits();

        if(temp != pastState)
        {
            m_debugSelectionsList->clearContents();
            m_debugSelectionsList->setRowCount(currentGame->getSelectedUnits().size());

            for(auto& unit : temp)
            {
               m_debugSelectionsList->setItem(i++, 0, new QTableWidgetItem(QVariant(unit).toString() ));
            }

            pastState = temp;
        }

    }
  }

  void _GUI::updateDebugUnitFocus()
  {
    QTableWidgetItem* top = m_debugSelectionsList->item(0, 0);

    if(top != NULL)
        m_debugSelectionsList->setCurrentCell(0, 0);
  }

  void _GUI::updateDebugInfoTable()
  {
    QList<QTableWidgetItem *> selectionList = m_debugSelectionsList->selectedItems();
    int i = 0;

    m_debugSelectionInfo->clearContents();

    if(!selectionList.empty() && AnimationEngine->GetCurrentGame() != NULL && AnimationEngine->GetCurrentFrame() != NULL)
    {

        Frame * currentFrame = AnimationEngine->GetCurrentFrame();
        int index = selectionList.first()->text().toInt();


        if(currentFrame->unitAvailable(index))
        {

            std::map<std::string, QVariant>& parameters = (*currentFrame)[selectionList.first()->text().toInt()];

            m_debugSelectionInfo->setRowCount(parameters.size());

            for(auto& param: parameters)
            {
                m_debugSelectionInfo->setItem(i, 0, new QTableWidgetItem(QString(param.first.c_str())));
                m_debugSelectionInfo->setItem(i++, 1, new QTableWidgetItem(param.second.toString()));
            }

        }

    }
  }

  int _GUI::getCurrentUnitFocus()
  {
    static int callCount = 0;
    auto selection = m_debugSelectionsList->selectedItems();

    if(selection.size() != 0)
    {
        return selection.first()->text().toInt();
    }
    else
    {
        return -1;
    }
  }

  int _GUI::getDebugOptionState(const std::string& option)
  {
    int value = -1;
    for(auto& it : m_debugOptions)
    {
        if(it.first == option)
        {
            value = it.second->isChecked();
            break;
        }
    }
    return value;
  }

  void _GUI::frameChanged()
  {
    IGame* currentGame = AnimationEngine->GetCurrentGame();

    if(currentGame != NULL)
        currentGame->pruneSelection();
  }

  void _GUI::debugOptionStateChanged(int)
  {
    IGame* currentGame = AnimationEngine->GetCurrentGame();

    if(currentGame != NULL)
        currentGame->optionStateChanged();
  }

  void _GUI::closeGUI()
  {
    close();
  }

  void _GUI::toggleFullScreen()
  {
    setFullScreen(!fullScreen);
  }

  void _GUI::showDebugWindow()
  {
    m_dockWidget->show();
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
    if( value < 0 )
      value = 9;
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

  void _GUI::sliderChanged(int value)
  {
    TimeManager->pause();
    TimeManager->setTurnPercent( float(value)/1000 );
  } // _GUI::sliderChanged()

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

  void _GUI::splashScreen()
  {
    m_chooseDialog = new QDialog(this, Qt::SplashScreen);
    m_chooseDialog->setLayout( new QBoxLayout( QBoxLayout::LeftToRight ) );

    QPushButton *load = new QPushButton( "Load Gamelog" );
    QPushButton *spectate = new QPushButton( "Spectate/Play" );

    m_chooseDialog->layout()->addWidget( load );

    for(auto& i: Games->gameList())
    {
      if(i->getPluginInfo().spectateMode)
      {
        m_chooseDialog->layout()->addWidget( spectate );
        break;
      }

    }


    GUI->connect( load, SIGNAL( clicked() ), GUI, SLOT( fileOpen() ) );
    GUI->connect( load, SIGNAL( clicked() ), m_chooseDialog, SLOT( close() ) );
    GUI->connect( spectate, SIGNAL( clicked() ), GUI, SLOT( fileSpectate() ) );
    GUI->connect( spectate, SIGNAL( clicked() ), m_chooseDialog, SLOT( close() ) );

    m_chooseDialog->show();
    m_chooseDialog->move( QApplication::desktop()->screenGeometry(0).center() - m_chooseDialog->rect().center() );

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

