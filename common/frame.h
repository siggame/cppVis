#ifndef FRAME_H
#define FRAME_H

#include "common.h"
#include "animatable.h"
#include <list>
#include <string>
#include <map>

namespace visualizer
{
  class Frame
  {
    public:
      Frame()
      {
      }

      Frame( const Frame& frame )
      {
        m_frame = frame.m_frame;
        m_unitInfo = frame.m_unitInfo;

      }

      virtual ~Frame() {}

      void addAnimatable( const SmartPointer<Animatable>& animatable );
      void addAnimatableFront( const SmartPointer<Animatable>& animatable );

      bool unitAvailable( const int& unitId )
      {
        return (m_unitInfo.find( unitId ) != m_unitInfo.end());
      }

      map<string, string>& operator[] ( const int& unitId )
      {
        return m_unitInfo[unitId];
      }

      const map<string, string>& operator[] ( const int& unitId ) const
      {
        return m_unitInfo.find( unitId )->second;
      }

      /// @TODO Should probably return a wrapped object
      std::list<SmartPointer<Animatable> >& getAnimations();
      size_t size() const;

    private:
      std::list<SmartPointer<Animatable> > m_frame;
      std::map<int, map<string,string> > m_unitInfo;

  }; // Frame

} // visualizer

#endif
