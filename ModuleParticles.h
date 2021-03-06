#ifndef __MODULE_PARTICLES_H__
#define __MODULE_PARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include "Particle.h"
#include "Collider.h"
#include "Game.h"

#define MAX_ACTIVE_PARTICLES 2500

struct SDL_Texture;
struct Collider;

class ModuleParticles : public Module
{
public:
    // Constructor
    // Initializes all the particles in the array to nullptr
    ModuleParticles(bool startEnabled);

    //Destructor
    ~ModuleParticles();

    // Called when the module is activated
    // Loads the necessary textures for the particles
    bool Start() override;

    // Called at the middle of the application loop
    // Iterates all the particles and calls its Update()
    // Removes any "dead" particles
    UPDATE_STATUS Update() override;

    // Called at the end of the application loop
    // Iterates all the particles and draws them
    UPDATE_STATUS PostUpdate() override;

    // Called on application exit
    // Destroys all active particles left in the array
    bool CleanUp() override;

    // Creates a new particle and adds it to the array
    // Param particle	- A template particle from which the new particle will be created
    // Param x, y		- Position x,y in the screen (upper left axis)
    // Param delay		- Delay time from the moment the function is called until the particle is displayed in screen
    Particle* AddParticle(const Particle& particle, int x, int y, Collider::TYPE colliderType = Collider::TYPE::NONE, uint delay = 0);

    // Collision callback, called when the player intersects with another collidernormalWire
    void OnCollision(Collider* c1, Collider* c2) override;

    int GetParticleCounter() const;
    
private:
    // An array to store and handle all the particles
    Particle* particles[MAX_ACTIVE_PARTICLES] = { nullptr };

    // An index to the last added particle
    uint lastParticle = 0;
    int particleCounter = 0;

    uint vulcanCeilingSoundIndex;
    uint powerWireSoundIndex;

    // The particle's collider
    Collider* collider = nullptr;

    // A flag to detect when the particle has been destroyed
    bool destroyed = false;
    bool oncePowerWire = true;
    SDL_Rect finalPowerWireSrc = {};
    SDL_Rect finalPowerWireDst = {};

public:
    //Shots (and others) go here
    Particle normalWire;
    Particle powerWire;
    Particle hookPowerWire;
    Particle powerShot;

    Particle rBigBalloonExplosion;
    Particle rNotThatMehBalloonExplosion;
    Particle rMehBalloonExplosion;
    Particle rSmolBalloonExplosion;

    Particle bBigBalloonExplosion;
    Particle bNotThatMehBalloonExplosion;
    Particle bMehBalloonExplosion;
    Particle bSmolBalloonExplosion;

    Particle gBigBalloonExplosion;
    Particle gNotThatMehBalloonExplosion;
    Particle gMehBalloonExplosion;
    Particle gSmolBalloonExplosion;

    Particle muzzleFlash;
    Particle vulcanCeiling;
    Particle hitScreen;
    Particle ready;
};

#endif // !__MODULEPARTICLES_H__