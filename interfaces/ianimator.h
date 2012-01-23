#ifndef IANIMATOR_H
#define IANIMATOR_H

#include "common.h"

namespace visualizer
{

  class IGame;

  struct AnimData
  {
  }; // AnimData
 
  struct Anim
  {
    virtual float controlDuration() const = 0;
    virtual float totalDuration() const = 0;
    float startTime; 
    float endTime;

    virtual void animate( const float& t, AnimData *d, IGame* game ) = 0;

    virtual ~Anim() {}
  }; // Anim

} // visualizer

#endif // IANIMATOR_H
