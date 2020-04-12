#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"
#include "Animation.h"

struct SDL_Texture;
struct Collider;

class Enemy {
public:
    // Constructor
    // Saves the spawn position for later movement calculations
    Enemy(int x, int y);

    // Destructor
    virtual ~Enemy();

    // Returns the enemy's collider
    Collider* GetCollider() const;

    // Called from inhering enemies' Udpate
    // Updates animation and collider position
    virtual void Update();

    // Called from ModuleEnemies' Update
    virtual void Draw();

    // Collision response
    // Triggers an animation and a sound fx
    virtual void OnCollision(Collider* c2) = 0;

    void SetLethality(bool _lethality);
    bool GetLethality();

public:
    // The current position in the world
    iPoint position;

    // The enemy's texture
    SDL_Texture* texture = nullptr;

    // Sound fx when destroyed
    int destroyedFx = 0;

protected:
    // A ptr to the current animation
    Animation* currentAnim = nullptr;

    // The enemy's collider
    Collider* collider = nullptr;

    // A bool to know if the collision is lethal for the enemy
    bool lethal = false;

    // Original spawn position. Stored for movement calculations
    iPoint spawnPos;
};

#endif // __ENEMY_H__