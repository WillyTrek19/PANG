#include "ModulePlayer.h"

#include "Game.h"

#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEnemies.h"
#include "ModuleParticles.h"
#include "ModuleCollisions.h"
#include "ModuleTransition.h"
#include "ModuleLevelOne.h"
#include "ModuleFonts.h"

#include "Particle.h"

#include "SDL/include/SDL_timer.h"
#include "SDL/include/SDL_scancode.h"
#include <stdio.h>

ModulePlayer::ModulePlayer(bool startEnabled) : Module(startEnabled) {
    //Animation setter
    idle.PushBack({ 12,112,25,32 });
    idle.SetSpeed(0.0f);

    shoot.PushBack({ 44,112,27,32 });
    shoot.SetSpeed(0.0f);

    moving.PushBack({ 12,2,29,32 });
    moving.PushBack({ 46,2,30,32 });
    moving.PushBack({ 80,2,29,32 });
    moving.PushBack({ 114,2,27,32 });
    moving.PushBack({ 148,2,29,32 });
    moving.SetSpeed(0.2f);

    ded.PushBack({ 81,112,40,29 });
    ded.SetSpeed(0.0f);

    gameOver = { 5,5,532,58 };
    timeOver = { 5,69,532,58 };
    ready = { 5,132,203,67 };
    life = { 154,44,16,16 };

    
}
ModulePlayer::~ModulePlayer() {}

bool ModulePlayer::Start() {
    LOG("Loading player textures");
    destroyed = false;
    isTimeOver = false;
    onceTimeIsOver = 0;
    once = true;
    onceMusic = true;
    onceHurry0 = true;
    onceHurry1 = true;
    onceHurry2 = true;
    godMode = false;
    onceDeath = true;
    onceDeathSpaguett = true;

    blueText = game->GetModuleTextures()->Load("Resources/Sprites/blueText.png");
    texture = game->GetModuleTextures()->Load("Resources/Sprites/player.png"); // arcade version
    shotSoundIndex = game->GetModuleAudio()->LoadFx("Resources/SFX/shotClaw.wav");
    dedSoundIndex = game->GetModuleAudio()->LoadFx("Resources/SFX/dead.wav");

    position.x = 163;
    position.y = 186 - idle.GetHeight();
    mruaSpeed.x = 100;
    mruaSpeed.y = -197;
    physics.SetAxis(false, false); // physics

    collider = game->GetModuleCollisions()->AddCollider({ position.x, position.y, idle.GetWidth(), idle.GetHeight() }, Collider::TYPE::PLAYER, this); // adds a collider to the player

    normalFont = game->GetModuleFonts()->Load("Resources/Sprites/Font.png", "a ',.0123456789:bcdefghijklmnopqrstuvwxyz-", 6);
    timerFont = game->GetModuleFonts()->Load("Resources/Sprites/Timer.png", "time:0123456789 ", 2);
    timer = 100;
    time = -31.0f / 60.0f;

    return true;
}

UPDATE_STATUS ModulePlayer::Update()
{
    GamePad& pad = game->GetModuleInput()->GetGamePad(0);

    if (returnToIdle == 0) { currentAnimation = &idle; }
    else { --returnToIdle; }

    if (game->GetModuleLevelOne()->CheckIfStarted()) {
        //Reset the currentAnimation back to idle before updating the logic
        if (!destroyed && !isTimeOver) {
            if (((game->GetModuleInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
                && game->GetModuleInput()->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
                || (game->GetModuleInput()->GetGamePad().right
                && !game->GetModuleInput()->GetGamePad().left)
                || game->GetModuleInput()->GetGamePad().l_x > 0)
                && (returnToIdle == 0)) {
                currentAnimation = &moving;
                if (GetInvertValue()) { ChangeInvert(); }
                position.x += speed;
            }
            if (((game->GetModuleInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT
                && game->GetModuleInput()->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
                || (game->GetModuleInput()->GetGamePad().left
                && !game->GetModuleInput()->GetGamePad().right)
                || game->GetModuleInput()->GetGamePad().l_x < 0)
                && (returnToIdle == 0)) {
                currentAnimation = &moving;
                if (!(GetInvertValue())) { ChangeInvert(); }
                position.x -= speed;
            }

            // we need to activate it only when the player is on the stairs (OnCollision)
            /*if (game->GetModuleInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT 
                && game->GetModuleInput()->GetKey(SDL_SCANCODE_S) != KEY_REPEAT
                && game->GetModuleInput()->GetGamePad().up
                && game->GetModuleInput()->GetGamePad().down
                && game->GetModuleInput()->GetGamePad().l_y < 0
                (returnToIdle == 0)) {
                currentAnimation = &idle;
                if (!(GetInvertValue())) { ChangeInvert(); }
                position.y -= speed;
            }
            if (game->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT
                && game->GetModuleInput()->GetKey(SDL_SCANCODE_W) != KEY_REPEAT
                && game->GetModuleInput()->GetGamePad().down
                && game->GetModuleInput()->GetGamePad().up
                && game->GetModuleInput()->GetGamePad().l_y > 0
                && (returnToIdle == 0)) {
                currentAnimation = &idle;
                if (!(GetInvertValue())) { ChangeInvert(); }
                position.y += speed;
            }*/

            if ((game->GetModuleInput()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN
                || game->GetModuleInput()->GetGamePad().x
                || game->GetModuleInput()->GetGamePad().b
                || game->GetModuleInput()->GetGamePad().r2 > 0)
                && shot == false) {
                shot = true;
                if (!godMode) { game->GetModuleAudio()->PlayFx(shotSoundIndex); }
                currentAnimation = &shoot;

                if (GetInvertValue()) {
                    game->GetModuleParticles()->AddParticle(game->GetModuleParticles()->normalWire, position.x + (shoot.GetWidth() / 3), position.y - 1, Collider::TYPE::PLAYER_SHOT);
                    game->GetModuleParticles()->AddParticle(game->GetModuleParticles()->muzzleFlash, position.x + 3, position.y - 10, Collider::TYPE::PLAYER_SHOT);
                }
                if (!GetInvertValue()) {
                    game->GetModuleParticles()->AddParticle(game->GetModuleParticles()->normalWire, position.x + (shoot.GetWidth() / 2), position.y - 1, Collider::TYPE::PLAYER_SHOT);
                    game->GetModuleParticles()->AddParticle(game->GetModuleParticles()->muzzleFlash, position.x + currentAnimation->GetWidth() / 2 - 6, position.y - 10, Collider::TYPE::PLAYER_SHOT); //It works, it just works ~Todd Howard,from Skyrim
                }

                returnToIdle = 20;
            }
            if (currentAnimation == &idle) { collider->SetPos(position.x, position.y, idle.GetWidth(), idle.GetHeight()); }
            else if (currentAnimation == &moving) { collider->SetPos(position.x, position.y, moving.GetWidth(), moving.GetHeight()); }
            else if (currentAnimation == &shoot) { collider->SetPos(position.x, position.y, shoot.GetWidth(), shoot.GetHeight()); }
        }

        if (game->GetModuleInput()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) { godMode = !godMode; }
        if (game->GetModuleInput()->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) { destroyed = !destroyed; }
        if (game->GetModuleInput()->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {
            //!puppy
            game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleProjectSheet(), 90);
        }
        if (game->GetModuleInput()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) { game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleWinScreen(), 4); }
        if (game->GetModuleInput()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) { timer = 2; }

        if (isTimeOver) {
            if (once) {
                once = false;
                game->GetModuleAudio()->PlayMusicOnce("Resources/BGM/noMusic.ogg");
                game->GetModuleAudio()->PlayFx(dedSoundIndex);
            }
            if (playerLifes == 0) {
                if (onceMusic) {
                    game->GetModuleAudio()->PlayMusicOnce("Resources/BGM/gameOver.ogg");
                    onceMusic = false;
                }
                if (game->GetModuleAudio()->DetectIfEnd() == false) { game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleTitleScreen(), 4); }
            }
            else if (playerLifes >= 0 && onceTimeIsOver == 125) {
                if (onceDeathSpaguett) {
                    onceDeathSpaguett = false;
                    playerLifes--;
                }
                game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleLevelOne(), 4);
            }
        }

        if (destroyed) {
            if (once) {
                once = false;
                SDL_Delay(1000); //THIS SHIT HAS TO LEAVE AS SOON AS WE CAN. IT IS NO LONGER ALLOWED TO EXIST. THIS IS A NO SDL_DELAY ZONE. EVEN A FUCKING SECOND GIVES ME INSANE ANXIETY. AND ALSO AIDS. NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE NIGHTMARE. Oh hi Marc, fancy seeing you here :^3 - Luce <3
                game->GetModuleAudio()->PlayFx(dedSoundIndex);
                game->GetModuleParticles()->AddParticle(game->GetModuleParticles()->hitScreen, 0, 0);
                physics.SetAxis(true, true);
            }
            currentAnimation = &ded;
            if (position.y >= SCREEN_HEIGHT + currentAnimation->GetHeight() && playerLifes == 0) {
                if (onceMusic) {
                    game->GetModuleAudio()->PlayMusicOnce("Resources/BGM/gameOver.ogg");
                    onceMusic = false;
                }
                if (game->GetModuleAudio()->DetectIfEnd() == false) { game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleTitleScreen(), 4); }
            }
            else if (position.y >= SCREEN_HEIGHT + currentAnimation->GetHeight() && playerLifes > 0) {
                if (onceDeathSpaguett) {
                    onceDeathSpaguett = false;
                    playerLifes--;
                }
                game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleLevelOne(), 4);
            }
            physics.UpdatePhysics(position.x, position.y, mruaSpeed.x, mruaSpeed.y);
            collider->SetPos(position.x, position.y, ded.GetWidth(), ded.GetHeight());

            //Unknown Code By Luce
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //deltaTime = 1.0f / 60.0f;
            //if (time <= 10) { time += deltaTime; }
            //else {
            //    time = deltaTime;
            //    --timer;
            //    playerLifes = 2;
            //    game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleLevelOne(), 4);
            //}
        }

        currentAnimation->Update();

        if (game->GetModuleEnemies()->CheckForBalloons()) {
            if (winCountdown == 75) { game->GetModuleTransition()->Transition((Module*)game->GetModuleLevelOne(), (Module*)game->GetModuleWinScreen(), 4); }
            else { ++winCountdown; }
        }
    }
    return UPDATE_STATUS::UPDATE_CONTINUE;
}

UPDATE_STATUS ModulePlayer::PostUpdate() {
    sprintf_s(scoreText, 10, "%5d", score);
    game->GetModuleFonts()->BlitText(64, 202, normalFont, scoreText);
    sprintf_s(playerText, 10, "player-1");
    game->GetModuleFonts()->BlitText(16, 194, normalFont, playerText);

    for (int i = playerLifes; i > 0; i--) {
        SDL_Rect lifeAdapter = { i * 50,SCREEN_HEIGHT*3-50,16,16 };
        game->GetModuleRender()->Blit(texture, 0, 0, false, &life, &lifeAdapter);
    }

    game->GetModuleRender()->Blit(texture, position.x, position.y, GetInvertValue(), &currentAnimation->GetCurrentFrame());

    SDL_Rect backgroundAdapter = { 0, 0, 384, 193 };
    if (isTimeOver) {
        SDL_Rect timeOverAdapter = { (SCREEN_WIDTH / 2 + 150),backgroundAdapter.h + 75,150,17 };
        game->GetModuleRender()->Blit(blueText, 0, 0, false, &timeOver, &timeOverAdapter);
        onceTimeIsOver++;
    }
    else if (playerLifes == 0 && position.y >= SCREEN_HEIGHT + currentAnimation->GetHeight()) {
        SDL_Rect gameOverAdapter = { (SCREEN_WIDTH / 2 + 150),backgroundAdapter.h + 75,150,17 };
        game->GetModuleRender()->Blit(blueText, 0, 0, false, &gameOver, &gameOverAdapter);
    }

    //Timer
    sprintf_s(timerText, 10, "time:%03d", timer);
    game->GetModuleFonts()->BlitText(SCREEN_WIDTH - 129, 8, timerFont, timerText);

     if (!destroyed && !isTimeOver) {
         if (time <= 1) { time += deltaTime; }
         else {
             time = deltaTime;
             --timer;
         }

         if (timer <= 0) { isTimeOver = true; }

         if (timer == 20 && onceHurry2 == true) {
             onceHurry1 = false;
             onceHurry2 = false;
             game->GetModuleAudio()->PlayMusic("Resources/BGM/hurryUpLvl2.ogg");
         }

         if (timer <= 50 && onceHurry1 == true) {
             if (onceHurry0) {
                 onceHurry0 = false;
                 game->GetModuleAudio()->PlayMusicOnce("Resources/BGM/introHurryUpLvl1.ogg");
             }
             game->GetModuleAudio()->ChangeAtEnd("Resources/BGM/hurryUpLvl1.ogg");
         }
     }

    return UPDATE_STATUS::UPDATE_CONTINUE; 
}

void ModulePlayer::OnCollision(Collider* c1, Collider* c2) {
    short int counter = 0;
    if (!godMode) {
        if (c1 == collider && c2->GetType() == Collider::TYPE::WALL) {
            if (position.x < (c2->GetRect().x + c2->GetRect().w) && position.x > c2->GetRect().x) { 
                position.x = (c2->GetRect().x + c2->GetRect().w);
                if (destroyed) {
                    mruaSpeed.x = -mruaSpeed.x * 2;
                    ChangeInvert();
                }
            }
            else if ((position.x + currentAnimation->GetWidth()) > c2->GetRect().x&& position.x < c2->GetRect().x) { 
                position.x = (c2->GetRect().x - currentAnimation->GetWidth()); 
                if (destroyed) {
                    mruaSpeed.x = -mruaSpeed.x / 2;
                    ChangeInvert();
                }
            }
        }
        else if (c1 == collider && !destroyed && !isTimeOver && c2->GetType() == Collider::TYPE::STAIRS) {
            //if ((c2->GetRect().y + c2->GetRect().h) >= (position.y + currentAnimation->GetHeight())) { // (bottom of stairs) should be able to move both x and y

            //}
            //else if (c2->GetRect().y <= position.y) { // (top of stairs) should be able to move both x and y

            //}
            //else { // (middle of stairs) should only be able to move y

            //}
        }
        else if (c1 == collider && !destroyed && !isTimeOver && c2->GetType() == Collider::TYPE::BALLOON) { destroyed = true; }
        else if (c1 == collider && !destroyed && !isTimeOver && c2->GetType() == Collider::TYPE::ANIMAL) { /*destroyed = true;*/ }
        else if (destroyed) {
            if ((position.x + currentAnimation->GetWidth()) > c2->GetRect().x&& position.x < c2->GetRect().x) {
                mruaSpeed.x = -50;
            }
            if (c1 == collider && onceDeath && c2->GetType() == Collider::TYPE::FLOOR) {
               mruaSpeed.y = -145;
               onceDeath = false;
            }
        }
    }
}

int ModulePlayer::GetSpeed() const { return speed; }
void ModulePlayer::SetSpeed(int _speed) { speed = _speed; }
iPoint ModulePlayer::GetPosition() const { return position; }
void ModulePlayer::SetPosition(iPoint _position) { position = _position; }
SDL_Texture* ModulePlayer::GetTexture() const { return texture; }
SDL_Texture* ModulePlayer::GetBlueTextTexture() const { return blueText; }
Animation* ModulePlayer::GetCurrentAnimation() const { return currentAnimation; }
uint ModulePlayer::GetShotSoundIndex() const { return shotSoundIndex; }
void ModulePlayer::SetShotSoundIndex(uint _shotSoundIndex) { shotSoundIndex = _shotSoundIndex; }
uint ModulePlayer::GetDedSoundIndex() const { return shotSoundIndex; }
void ModulePlayer::SetDedSoundIndex(uint _dedSoundIndex) { dedSoundIndex = _dedSoundIndex; }
int ModulePlayer::GetFontIndex() const { return normalFont; }
int ModulePlayer::GetTimerFontIndex() const { return timerFont; }
void ModulePlayer::SetIfShot(bool _shot) { shot = _shot; }
bool ModulePlayer::CheckIfGodMode() const { return godMode; };
bool ModulePlayer::CheckIfDestroyed() const { return (destroyed || isTimeOver); };
void ModulePlayer::AddScore(int _score) { score += _score; }
int ModulePlayer::GetTimer() const { return timer; }
void ModulePlayer::SetPlayerLives(int lives) { playerLifes = lives; }
void ModulePlayer::SetScore(int _score) { score = _score; };