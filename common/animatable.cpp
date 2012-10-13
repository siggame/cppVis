#include "animatable.h"
#include "ianimationengine.h"

namespace visualizer
{

  void Animatable::addKeyFrame( SmartPointer< Anim > anim )
  {
    m_frames.push_back( SmartPointer<Anim>( anim ) );
    std::list<SmartPointer<Anim> >::iterator i = m_frames.end();
    i--;
    m_keyFrames.push_back( i );

  } // Animatable::addKeyFrame()

  void Animatable::addSubFrame( const size_t& subFrame, SmartPointer< Anim > anim )
  {
    m_frames.insert( m_keyFrames[ subFrame ], SmartPointer<Anim>( anim ) );

  } // Animatable::addSubFrame()

  size_t Animatable::numKeyFrames() const
  {
    return m_keyFrames.size();

  } // Animatable::numKeyFrames()

  size_t Animatable::numFrames() const
  {
    return m_frames.size();

  } // Animatable::numFrames()

  std::list<SmartPointer<Anim> >& Animatable::getFrames()
  {
    return m_frames;

  } // Animatable::getFrames()

  const std::list<SmartPointer<Anim> >& Animatable::getFrames() const
  {
    return m_frames;

  } // Animatable::getFrames() const

} // visualizer
