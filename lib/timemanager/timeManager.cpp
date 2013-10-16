#include "timeManager.h"
#include "../optionsmanager/optionsman.h"

#include <ctime>
#include <cmath>

// CHANGE ME CHANGE ME CHANGE ME
#include "gui/gui.h"

namespace visualizer
{

  _TimeManager *TimeManager = 0;

  void _TimeManager::setup()
  {

    if( !TimeManager )
      {
        TimeManager = new _TimeManager;
        TimeManager->_setup();
      }
  } // _TimeManager::setup()

  void _TimeManager::destroy()
  {
    delete TimeManager;
    TimeManager = 0;

  } // _TimeManager::destroy()

  void _TimeManager::_setup()
  {
    MESSAGE( "============Initializing TimeManager=======" );
    /// @FIXME There's really no reason this shouldn't all be in a constructor
    m_turn = 0;
    m_progress = 0;
    m_speedModifier = 0;
    m_turnCompletion = 0;
    m_numTurns = 1;
    m_maxTurns = -1;
    m_breakout = true;
    loop_on = false;
    loop_start = m_turn;
    loop_end = m_numTurns;

  } // _TimeManager::_setup()

  void _TimeManager::updateProgress( float progress )
  {
    m_progress = progress;

  } // _TimeManager::updateProgress()

  void _TimeManager::setSpeed( const float& speed )
  {
    m_speedModifier = speed;

  } // _TimeManager::setSpeed()

  void _TimeManager::timerStart()
  {
    start();

  } // _TimeManager::timerStart()

  void _TimeManager::start()
  {
    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL(timeout()), this, SLOT(timerUpdate()) );
    int maxFPS = 60;
    // Determines the refresh rate based off of the max number of frames/sec.
    m_timer->start( (float)1000/maxFPS );
    m_time.start();

  } // _TimeManager::start()

  void _TimeManager::requestUpdate( UpdateNeeded* requester )
  {
    m_updateRequesters.push_back( requester );

  } // _TimeManager::requestUpdate()

  void _TimeManager::removeRequest( UpdateNeeded* requester )
  {
    for
      (
      std::list< UpdateNeeded* >::iterator i = m_updateRequesters.begin();
      i != m_updateRequesters.end();
      i++
      )
    {
      if( requester == *i )
      {
        m_updateRequesters.erase( i );
        return;
      }
    }

#ifdef __DEBUG__
  WARNING( "Could not find the requester." );

#endif

  } // _TimeManager::requestUpdate()


  void _TimeManager::updateChildren()
  {
    for(
      std::list< UpdateNeeded* >::iterator i = m_updateRequesters.begin();
      i != m_updateRequesters.end();
      i++
      )
    {
      (*i)->update();
    }

  } // _TimeManager::updateChildren()

  const int& _TimeManager::getTurn()
  {
    if( m_turnCompletion < 0 )
    {
      setTurn(m_turn - 1);
    }

    if( m_turn < 0 )
      setTurn(0);

    if( (unsigned int)m_turn >= m_maxTurns )
    {
      setTurn(m_maxTurns - 1);
    }

    return m_turn;

  } // _TimeManager::getTurn()

  void _TimeManager::setTurnPercent( const float& perc )
  {
    m_turnCompletion = perc;
    if( m_turnCompletion < 0 )
    {
      m_turnCompletion += 1;
      setTurn(m_turn - 1);
    } else if( m_turnCompletion >= 1 )
    {
      m_turnCompletion -= 1;
      setTurn(m_turn + 1);
    }


  } // _TimeManager::setTurnPercent()

  const float& _TimeManager::getTurnPercent()
  {
    if( m_turnCompletion < 0 )
    {
      m_turnCompletion += 1;
      setTurn(m_turn - 1);
    } else if( m_turnCompletion >= 1 )
    {
      m_turnCompletion -= 1;
      setTurn(m_turn + 1);
    }

    return m_turnCompletion;

  } // _TimeManager::getTurnPercent()

  void _TimeManager::setTurn( const int& turn )
  {
    if(turn != m_turn)
        emit TurnChanged();

    m_turn = turn;

    //updateChildren();

  } // _TimeManager::setTurn()

  const int& _TimeManager::nextTurn()
  {
    pause();
    setTurn(m_turn + 1);
    return getTurn();

  } // _TimeManager::nextTurn()

    const int& _TimeManager::prevTurn()
  {
    pause();
    setTurn(m_turn - 1);
    return getTurn();

  } // _TimeManager::prevTurn()

  void _TimeManager::play()
  {
    m_speedModifier = 1;

  } // _TimeManager::play()

  void _TimeManager::pause()
  {
    m_speedModifier = 0;

    m_turnCompletion = 0.99999f;

  } // _TimeManager::pause()

  void _TimeManager::abloop_on(const int& a, const int&b)
  {
	  loop_on = true;
	  loop_start = a;
	  loop_end = b;
	  setTurn(a);
	  play();
  }
  void _TimeManager::abloop_off()
  {
	  loop_on = false;
  }

  void _TimeManager::fastForward()
  {
    m_speedModifier++;

  } // _TimeManager::fastForward()

  void _TimeManager::rewind()
  {
    if( m_speedModifier >= 0 )
    {
      m_speedModifier = -1;
    }
    else
    {
      m_speedModifier--;
    }

  } // _TimeManager::rewind()

  char _TimeManager::readyForGamelog()
  {
    return (m_speedModifier == 0);

  } // _TimeManager::readyForGamelog()

  const int& _TimeManager::getNumTurns() const
  {
    return m_numTurns;

  } // _TimeManager::getNumTurns()

  void _TimeManager::setNumTurns( const int& numTurns )
  {
    m_numTurns = numTurns;

    // @FIXME Must update control bar
    // @FIXME
    //updateChildren();

  } // _TimeManager::setNumTurns()

  void _TimeManager::setMaxTurns( const size_t& maxTurns )
  {
    m_maxTurns = maxTurns;

  }

  float _TimeManager::getSpeed()
  {
    // TODO: Make this a logarithmic increase
    return OptionsMan->getNumber( "Speed" ) * m_speedModifier / 3;

  } // _TimeManager::getSpeed()

  void _TimeManager::timerUpdate()
  {
    // m_time.restart() returns the number of milliseconds
    // since it was last called.

    float secElapsed = (float)m_time.restart()/1000;
    m_turnCompletion += getSpeed() * secElapsed;

    if( m_turn < 0 )
    {
      setTurn(0);
    }

    if( m_turnCompletion > 1 )
    {
      size_t skip = floor( m_turnCompletion );
      setTurn(m_turn + skip);
      m_turnCompletion -= skip;

      if(loop_on && m_turn >= loop_end)
      {
        setTurn(loop_start);
      }

      if( (unsigned int)m_turn >= m_maxTurns )
      {
        setTurn(m_maxTurns - 1);
      }

      if( m_turn >= m_numTurns-1 && !m_breakout )
      {
        pause();

        setTurn(m_numTurns-1);
        // Very close to the next turn, but not quite
        m_turnCompletion = 0.9999f;

        if( !OptionsMan->getString( "Game Mode" ).compare( "Arena" ) )
        {
          if( !GUI->loadInProgress() )
          {
            m_breakout = true;
            GUI->requestGamelog();
          }
        } else if( OptionsMan->getNumber( "Continuous Play" ) )
        {
          GUI->playNext();
        }

      }
      else
      {
        if( m_turn >= m_numTurns-1 )
        {
          m_breakout = false;
          setTurn(m_numTurns - 1);
          m_turnCompletion = 0.01f;
        }
      }

    }
    else if( m_turnCompletion < -1 )
    {
      setTurn(m_turn + ceil( m_turnCompletion ));
      m_turnCompletion -= ceil( m_turnCompletion );

      if(loop_on && m_turn <= loop_start)
      {
        setTurn(loop_end);
      }

      if( m_turn <= 0 )
      {
        setTurn(0);
        m_turnCompletion = 0;
        pause();
      }

    }

    if( m_turn >= m_numTurns )
    {
      setTurn(m_numTurns - 1);
      m_turnCompletion = 0.9999f;
    }

    updateChildren();

  } // _TimeManager::timerUpdate()


} // visualizer

