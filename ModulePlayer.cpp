#include "Game.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleParticles.h"
#include "Particle.h"
#include "ModuleCollisions.h"

#include "SDL/include/SDL_scancode.h"

ModulePlayer::ModulePlayer() {
    //Animation setter
    idle.PushBack({ 12,112,25,31 });
    idle.SetSpeed(0.0f);

    shoot.PushBack({ 44,115,27,28 });
    shoot.SetSpeed(0.0f);

    moving.PushBack({ 12,2,29,31 });
    moving.PushBack({ 46,3,30,30 });
    moving.PushBack({ 80,2,29,31 });
    moving.PushBack({ 114,3,27,30 });
    moving.PushBack({ 148,3,29,30 });
    moving.SetSpeed(0.2f);
}
ModulePlayer::~ModulePlayer() {}

int ModulePlayer::GetSpeed() const { return speed; }
void ModulePlayer::SetSpeed(int _speed) { speed = _speed; }
iPoint ModulePlayer::GetPosition() const { return position; }
void ModulePlayer::SetPosition(iPoint _position) { position = _position; }
SDL_Texture* ModulePlayer::GetTexture() const { return texture; }
uint ModulePlayer::GetShotSoundIndex() const { return shotSoundIndex; }
void ModulePlayer::SetShotSoundIndex(uint _shotSoundIndex) { shotSoundIndex = _shotSoundIndex; }


bool ModulePlayer::Start()
{
    LOG("Loading player textures");

    texture = game->textures->Load("Resources/Sprites/Player.png"); // arcade version
    shotSoundIndex = game->audio->LoadFx("Resources/SFX/shotClaw.wav");

    position.x = 177;
    position.y = 186;

    collider = game->collisions->AddCollider({ position.x, position.y, idle.GetWidth(), -idle.GetHeight() }, Collider::Type::PLAYER, this); // adds a collider to the player

    return true;
}

UPDATE_STATUS ModulePlayer::Update()
{
    //Reset the currentAnimation back to idle before updating the logic
    if (returnToIdle == 0) { currentAnimation = &idle; }
    else { --returnToIdle; }

    if (game->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && (returnToIdle == 0)) {
        currentAnimation = &moving;
        if (GetInvertValue()) { ChangeInvert(); }
        position.x += speed;
    }
    if (game->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && (returnToIdle == 0)) {
        currentAnimation = &moving;
        if (!(GetInvertValue())) { ChangeInvert(); }
        position.x -= speed;
    }
    if (game->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
        game->audio->PlayFx(shotSoundIndex);
        currentAnimation = &shoot;
        
        if (GetInvertValue()) {
            game->particles->AddParticle(game->particles->NormalWire, position.x + (shoot.GetWidth() / 3), position.y - shoot.GetHeight()-3);
        }
        if (!GetInvertValue()) {
            game->particles->AddParticle(game->particles->NormalWire, position.x + (shoot.GetWidth() / 2), position.y - shoot.GetHeight()-3);
        }

        returnToIdle = 5;
    }

    collider->SetPos(position.x, position.y);
    currentAnimation->Update();

    if (destroyed) {
        destroyedCountdown--;
        if (destroyedCountdown <= 0) { return UPDATE_STATUS::UPDATE_STOP; }
    }

    return UPDATE_STATUS::UPDATE_CONTINUE;
}

UPDATE_STATUS ModulePlayer::PostUpdate() {
    if (!destroyed) {
        SDL_Rect rect = currentAnimation->GetCurrentFrame();
        game->render->Blit(texture, position.x, position.y - rect.h, GetInvertValue(), &rect);
    }

    return UPDATE_STATUS::UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(Collider* c1, Collider* c2) {
    if (c1 == collider && destroyed == false) {
        destroyed = true;
    }
}