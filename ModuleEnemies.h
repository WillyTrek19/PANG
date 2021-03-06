#ifndef __MODULE_ENEMIES_H__
#define __MODULE_ENEMIES_H__

#include "Module.h"
#include "Enemy.h"

#define MAX_ENEMIES 100

struct EnemySpawnpoint {
    ENEMY_TYPE type = ENEMY_TYPE::NO_TYPE;
    int x = 0;
    int y = 0;
    bool isMovingRight = false;
};

class Enemy;
struct SDL_Texture;
struct Collider;
enum ENEMY_TYPE;

class ModuleEnemies : public Module {
public:
    // Constructor
    ModuleEnemies(bool startEnabled);

    // Destructor
    ~ModuleEnemies();

    // Called when the module is activated
    // Loads the necessary textures for the enemies
    bool Start() override;

    // Called at the middle of the application loop
    // Handles all enemies logic and spawning/despawning
    UPDATE_STATUS Update() override;

    // Called at the end of the application loop
    // Iterates all the enemies and draws them
    UPDATE_STATUS PostUpdate() override;

    // Called on application exit
    // Destroys all active enemies left in the array
    bool CleanUp() override;

    // Called when an enemi collider hits another collider
    // The enemy is destroyed and an explosion particle is fired
    void OnCollision(Collider* c1, Collider* c2) override;

    // Add an enemy into the queue to be spawned later
    bool AddEnemy(ENEMY_TYPE type, int x, int y, bool isMovingRight = false);

    // Iterates the queue and checks for camera position
    void HandleEnemiesSpawn();

    // Checks if there're any balloons onscreen
    bool CheckForBalloons();
    

private:
    // Spawns a new enemy using the data from the queue
    void SpawnEnemy(const EnemySpawnpoint& info);

    // A queue with all spawn points information
    EnemySpawnpoint spawnQueue[MAX_ENEMIES];

    // All spawned enemies in the scene
    Enemy* enemies[MAX_ENEMIES] = { nullptr };

    // The enemies sprite sheet
    SDL_Texture* enemyTexture = nullptr;

    // The audio fx for destroying an enemy
    uint balloogiExplosioni;

    bool onceSpawn = true;
};

#endif // __MODULE_ENEMIES_H__