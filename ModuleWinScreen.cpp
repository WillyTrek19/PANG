#include "ModuleWinScreen.h"

#include "Game.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleTransition.h"
#include "ModulePlayer.h"
#include "ModuleFonts.h"

#include "SDL/include/SDL_scancode.h"
#include <stdio.h>

ModuleWinScreen::ModuleWinScreen(bool startEnabled) : Module(startEnabled) {
    //SplashArt Pushbacks
    for (int i = 0; i != 29; ++i) { splashArt.PushBack({ i * 300,22,300,178 }); }
    splashArt.SetSpeed(0.5f);
    splashArt.SetLoop(true);
}

ModuleWinScreen::~ModuleWinScreen() {}

// Load assets
bool ModuleWinScreen::Start() {
    LOG("Loading background assets");

    splashArt.Reset();

    splashArtTexture = game->GetModuleTextures()->Load("Resources/Sprites/FirstStageSplashArt.png");
    game->GetModuleAudio()->PlayMusicOnce("Resources/BGM/levelComplete.ogg");

    return true;
}

UPDATE_STATUS ModuleWinScreen::Update() {
    splashArt.Update();
    game->GetModuleAudio()->ChangeModuleAtEnd("Resources/BGM/noMusic.ogg");

    return UPDATE_STATUS::UPDATE_CONTINUE;
}

// Update: draw background
UPDATE_STATUS ModuleWinScreen::PostUpdate() {
    // Draw everything --------------------------------------
    sprintf_s(stage, 10, "1stage");
    /*game->GetModuleFonts()->BlitText(160, 111, game->GetModulePlayer()->GetFontIndex(), stage);
    sprintf_s(timeBonus, 10, "time bonus:    %5d pts.", (game->GetModulePlayer()->GetTimer()));
    game->GetModuleFonts()->BlitText(159, 135, game->GetModulePlayer()->GetFontIndex(), timeBonus);
    sprintf_s(nextExtend, 10, "next extend:  %6d pts.", nextExtendNumber);
    game->GetModuleFonts()->BlitText(152, 161, game->GetModulePlayer()->GetFontIndex(), nextExtend);*/
    //Edit arguments of Blit
    SDL_Rect backgroundAdapter = { 0, 0, 384, 193 };
    SDL_Rect splashArtAdapter = { (SCREEN_WIDTH / 2 + 100),backgroundAdapter.h-100,200,95 };
    game->GetModuleRender()->Blit(splashArtTexture,0,0,false,&splashArt.GetCurrentFrame(),&splashArtAdapter);
    
    return UPDATE_STATUS::UPDATE_CONTINUE;
}

bool ModuleWinScreen::CleanUp() {
    game->GetModuleTextures()->Unload(splashArtTexture);
    return true;
}