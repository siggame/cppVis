///////////////////////////////////////////////////////////////////////////////
/// @file animationengine.h
/// @description Contains the class declaration for the Core Animation engine
///////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONENGINE_H
#define ANIMATIONENGINE_H

//#include "animator.h"
#include "../timemanager/timeManager.h"
#include "../../interfaces/igame.h"
#include "common.h"

#include <QtGui>

namespace visualizer
{

  ///////////////////////////////////////////////////////////////////////////////
  /// @class _AnimationEngine
  /// @brief The core animation engine for the visualizer.
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  /// @fn _AnimationEngine::getAnimator()
  /// @brief This function follows the abstract factory pattern.  It returns the
  /// latest and greatest animator class.
  /// @return SmartPointer<IAnimator> is a smart pointer which holds an instance
  /// of an actual animator
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  /// @fn _AnimationEngine::buildAnimation( Frame& frame )
  /// @brief This function must be applied to any frame before the animation
  /// engine can handle it properly.  It segments the animations for use in the
  /// renderer
  /// @param frame The Frame that we're going to loop through the animations in
  /// and segment.
  ///////////////////////////////////////////////////////////////////////////////

  // AnimationEngine will need to rely heavily on TimeManager
  // and trusting the renderer to give it the right frame.
  class _AnimationEngine: public QObject, public Module, public IAnimationEngine
  {
    Q_OBJECT;
    public:
      _AnimationEngine();

      static void setup();
      static void destroy();

      void buildAnimations( Frame& frame );

      void draw();
      void drawAnim( Animatable& animator );

      void registerGame( IGame* game, AnimSequence* frameList=0 );
      void registerFrameContainer( AnimSequence* frameList );

      void lock();
      void release();

      IGame* GetCurrentGame();

      Frame* GetCurrentFrame();

    signals:
      void NewGameLoaded(IGame * game);

    private:
      QMutex m_animMutex;
      AnimSequence *m_frameList;
      IGame *m_currentGame;

  }; // _AnimationEngine

  extern _AnimationEngine *AnimationEngine;

} // visualizer


#endif // ANIMATIONENGINE_H
