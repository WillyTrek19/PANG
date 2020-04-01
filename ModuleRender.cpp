#include "ModuleRender.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "SDL/include/SDL.h"

ModuleRender::ModuleRender() : Module() {}

ModuleRender::~ModuleRender() {}

bool ModuleRender::Init() {
    //LOG("Creating Renderer context");
    bool ret = true;
    Uint32 flags = 0;

    if (VSYNC == true) {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    renderer = SDL_CreateRenderer(App->window->window, -1, flags);

    if (renderer == nullptr) {
        //LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    testTexture = App->textures->Load("Assets/test.png");
    return ret;
}

// Called every draw update
update_status ModuleRender::PreUpdate() {
    //Set the color used for drawing operations
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //Clear rendering target
    SDL_RenderClear(renderer);

    return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate() {
    //Blit test texture
    Blit(testTexture, 243, 193, nullptr);

    //Update the screen
    SDL_RenderPresent(renderer);

    return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp() {
    //LOG("Destroying renderer");

    //Destroy the rendering context
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    return true;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section) {
    bool ret = true;
    SDL_Rect rect{ x, y, 0, 0 };
    if (section != nullptr) {
        rect.w = section->w;
        rect.h = section->h;
    }
    else {
        SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
    }

    if (SDL_RenderCopy(renderer, texture, section, &rect) != 0) {
        //LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
        ret = false;
    }

    return ret;
}