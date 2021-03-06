#ifndef ANIMATABLE_H
#define ANIMATABLE_H

#include "ianimator.h"
#include <vector>
#include <list>

namespace visualizer
{

  //////////////////////////////////////////////////////////////////////////////
  /// @class Animatable
  /// @brief Base Class for any object you want to associate animations with.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// @fn Animatable::getAnimationSequence( IAnimationEngine* animEngine )
  /// @brief Gets the IAnimator defined by the core engine and saves it.
  /// @param animEngine The interface to the core engine passed through on
  ///  initialization.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  /// @fn Animatable::getData()
  /// @brief Virtual function which gets the animation specific storage
  /// structure the animator uses.
  /// @return pointer to AnimData* which is just a base class.
  //////////////////////////////////////////////////////////////////////////////

  class IAnimationEngine;
  class IRenderer;

  class Animatable
  {
    public:
      Animatable(const std::string& tag = "") :
        m_renderTag(tag)
        {}

      virtual ~Animatable() {}
      virtual AnimData* getData()
      { return 0; }

      void addKeyFrame( SmartPointer< Anim > anim );
      void addSubFrame( const size_t& subFrame, SmartPointer< Anim > anim );

      size_t numKeyFrames() const;
      size_t numFrames() const;

      std::list<SmartPointer<Anim> >& getFrames();
      const std::list<SmartPointer<Anim> >& getFrames() const;

      std::string getTag() const
      { return m_renderTag;}

    private:
      std::vector< std::list<SmartPointer<Anim> >::iterator > m_keyFrames;
      std::list<SmartPointer<Anim> > m_frames;
      std::string m_renderTag;

  }; // Animatable

} // visualizer

#endif // ANIMATABLE_H
