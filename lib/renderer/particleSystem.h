
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <map>
#include <string>

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
  
  // todo: implement
  class ParticleSystem
  {
	  public:
	  
	  ParticleSystem(const std::string& texture, int iMaxParticles, float fTimePerParticle);
	  virtual ~ParticleSystem();
	  
	  // Pops a particle from the dead vector pool and adds it to the alive vector pool
	  void AddParticle();
	  
	  virtual void InitParticle(Particle& out) = 0;
	  
	  void Update(float dt);
	  void Render();
	  
	  private:
	  
	  std::string m_texture;
	  const int m_uiMaxParticles;
	  float m_fTime; // Age of ps
	  float m_fTimePerParticle; // time before the system emits another particle
	  
	  std::vector<Particle> m_Particles; // all particles
	  
	  // These pools are rebuilt every frame
	  std::vector<Particle> m_AliveParticles; // All particles that get rendered
	  std::vector<Particle> m_DeadParticles;
  };
  
  class ParticleSystemManager
  {
    public:
    
    ~ParticleSystemManager();
    
    /* todo: implement
    
    if the system does not exist, create it,
    then return the reference to the ps 
    
    */
    ParticleSystem& GetPS(const std::string& ps);
    
    // todo: add more methods
    
    private:
    
    std::map<std::string,ParticleSystem*> m_ParticleSystems;
    
    // todo: more stats could go here
  
  
  };


} // visualizer

#endif // PARTICLE_SYSTEM_H
