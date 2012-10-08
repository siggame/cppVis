#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <string>

// This will probably act more like a particle system manager than becoming the particle system
// itself.

namespace visualizer
{
  struct Vector2
  {
    float x;
    float y;
  };

  // P(t) = 0.5*a*t*t + v*t + p0
  // V(t) = a*t + v0
  struct Particle
  {
    Vector2 initPos;
    Vector2 initVelocity;
    float initTime;
    float lifeTime;
    float initSize;
  }; 

  class ParticleSystem
  {
	  public:
	  
	  ParticleSystem(const std::string& texture, int iMaxParticles, float fTimePerParticle);
	  virtual ~ParticleSystem();
	  
	  void AddParticle();
	  
	  virtual void InitParticle(Particle& out) = 0;
	  
	  void Update(float dt);
	  void Render();
	  
	  private:
	  
	  std::string m_texture;
	  int m_uiMaxParticles;
	  float m_fTime;
	  float m_fTimePerParticle; 
	  
	  std::vector<Particle> m_Particles;
	  std::vector<Particle> m_AliveParticles;
	  std::vector<Particle> m_DeadParticles;
  };


} // visualizer

#endif // PARTICLE_SYSTEM_H
