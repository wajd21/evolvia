#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "structp.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 800
#define FRAMES_PER_SECOND 30
#define NUM_FRAMES 2
#define FRAME_DELAY (1000 / FRAMES_PER_SECOND)

int main(void) {
    // Initialize SDL and subsystems
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(!IMG_Init(IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image initialization failed: %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if(TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Get player name
    char nom[30] = {0};
    printf("Enter your name: ");
    if(scanf("%29s", nom) != 1) {
        fprintf(stderr, "Error reading name\n");
        strncpy(nom, "Player", sizeof(nom)-1);
    }

    // Create screen
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if(!screen) {
        fprintf(stderr, "Screen creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("Platform Game", NULL);

    // Initialize game structures
    background c = {0};
    background1 c1 = {0}, c2 = {0};
    int partage = 0;
    int boucle = 1;
    SDL_Event event;

    // Set initial values
    c.walk_right_permission = 1;
    c.walk_left_permission = 1;
    c.acceleration = 1.0f;

    // Load backgrounds
    initbackground(&c);
    c1.menu2 = IMG_Load("back.jpg");
    c2.menu3 = IMG_Load("back.jpg");
    
    if(!c.menu1 || !c1.menu2 || !c2.menu3) {
        fprintf(stderr, "Error loading background images: %s\n", IMG_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    initcamera(&c1);
    initcameramultiplayer(&c2);

    // Initialize audio
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer initialization failed: %s\n", Mix_GetError());
    } else {
        musique(1);  // Play initial music
    }

    // Load fonts
    TTF_Font* font = TTF_OpenFont("font.ttf", 30);
    TTF_Font* button_font = TTF_OpenFont("font.ttf", 40);
    if(!font || !button_font) {
        fprintf(stderr, "Font loading failed: %s\n", TTF_GetError());
    }

    // Create button surfaces
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* button_surface = NULL;
    SDL_Surface* guide_surface = NULL;
    
    if(button_font) {
        button_surface = TTF_RenderText_Blended(button_font, "Scores", textColor);
        guide_surface = TTF_RenderText_Blended(button_font, "Guide", textColor);
    }

    SDL_Rect button_rect = {100, 50, button_surface ? button_surface->w : 0, button_surface ? button_surface->h : 0};
    SDL_Rect guide_rect = {100, 120, guide_surface ? guide_surface->w : 0, guide_surface ? guide_surface->h : 0};
    SDL_Rect control = {750, 0, 1500, 800};

    // Initialize animation
    Animation myanimation = {0};
    if(!init_animation(&myanimation)) {
        fprintf(stderr, "Animation initialization failed\n");
    }
    myanimation.pos.x = SCREEN_WIDTH/2 - 150;
    myanimation.pos.y = 0;

    Uint32 start_time = SDL_GetTicks();

    // Main game loop
    while(boucle) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 tempsEcoule = (currentTime - start_time) / 1000;

        // Clear screen
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        // Draw game elements
        afficher_background(screen, c);
        afficher_animation(myanimation, screen);
        
        if(button_surface) SDL_BlitSurface(button_surface, NULL, screen, &button_rect);
        if(guide_surface) SDL_BlitSurface(guide_surface, NULL, screen, &guide_rect);

        if(partage) {
            if(c1.menu2) SDL_BlitSurface(c1.menu2, &c1.camera1, screen, NULL);
            if(c2.menu3) SDL_BlitSurface(c2.menu3, &c2.camera1, screen, &control);
        }

        if(font) afficherTemps(screen, font, tempsEcoule);
        SDL_Flip(screen);

        // Event handling
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    boucle = 0;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if(button_surface && event.button.x >= button_rect.x && event.button.x < button_rect.x + button_rect.w &&
                       event.button.y >= button_rect.y && event.button.y < button_rect.y + button_rect.h) {
                        show_top_scores(screen);
                    }
                    if(guide_surface && event.button.x >= guide_rect.x && event.button.x < guide_rect.x + guide_rect.w &&
                       event.button.y >= guide_rect.y && event.button.y < guide_rect.y + guide_rect.h) {
                        show_guide(screen);
                    }
                    break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_p: partage = 1; break;
                        case SDLK_a: partage = 0; break;
                        case SDLK_ESCAPE: boucle = 0; break;
                        case SDLK_RIGHT: 
                            c.is_running_right = 1;
                            if(partage) c2.is_running_right = 1;
                            break;
                        case SDLK_d: c1.is_running_right = 1; break;
                        case SDLK_LEFT: 
                            c.is_running_left = 1;
                            if(partage) c2.is_running_left = 1;
                            break;
                        case SDLK_q: c1.is_running_left = 1; break;
                        case SDLK_LSHIFT:
                            c.shift_pressed = 1;
                            c.start_time = SDL_GetTicks();
                            break;
                        case SDLK_DOWN: 
                            scrollingdown(&c);
                            if(partage) scrollingdown1(&c2);
                            break;
                        case SDLK_UP: 
                            scrollingup(&c);
                            if(partage) scrollingup1(&c2);
                            break;
                        case SDLK_z: scrollingup1(&c1); break;
                        case SDLK_s: scrollingdown1(&c1); break;
                        default: break;
                    }
                    break;

                case SDL_KEYUP:
                    switch(event.key.keysym.sym) {
                        case SDLK_RIGHT: 
                            c.is_running_right = 0;
                            c2.is_running_right = 0;
                            break;
                        case SDLK_d: c1.is_running_right = 0; break;
                        case SDLK_LEFT: 
                            c.is_running_left = 0;
                            c2.is_running_left = 0;
                            break;
                        case SDLK_q: c1.is_running_left = 0; break;
                        case SDLK_LSHIFT: 
                        case SDLK_RSHIFT:
                            c.shift_pressed = c1.shift_pressed = c2.shift_pressed = 0;
                            c.acceleration = c1.acceleration = c2.acceleration = 1.0f;
                            break;
                        default: break;
                    }
                    break;
            }
        }

        // Movement updates
        if(c.is_running_right) scrolling_right(&c);
        if(c1.is_running_right) scrolling_right1(&c1);
        if(c2.is_running_right) scrolling_right1(&c2);
        if(c.is_running_left) scrolling_left(&c);
        if(c1.is_running_left) scrolling_left1(&c1);
        if(c2.is_running_left) scrolling_left1(&c2);

        // Acceleration updates
        if(c.shift_pressed && c.acceleration < 3.0f) {
            c.elapsed_time = SDL_GetTicks() - c.start_time;
            float new_acc = (c.elapsed_time / 750) * 0.5f + 1.0f;
            if(new_acc <= 3.0f) c.acceleration = new_acc;
        }
        if(c1.shift_pressed && c1.acceleration < 3.0f) {
            c1.elapsed_time = SDL_GetTicks() - c1.start_time;
            float new_acc = (c1.elapsed_time / 750) * 0.5f + 1.0f;
            if(new_acc <= 3.0f) c1.acceleration = new_acc;
        }
        if(c2.shift_pressed && c2.acceleration < 3.0f) {
            c2.elapsed_time = SDL_GetTicks() - c2.start_time;
            float new_acc = (c2.elapsed_time / 750) * 0.5f + 1.0f;
            if(new_acc <= 3.0f) c2.acceleration = new_acc;
        }

        SDL_Delay(16);  // Cap at ~60 FPS
    }

    // Save score
    Score score;
    strncpy(score.name, nom, sizeof(score.name)-1);
    score.name[sizeof(score.name)-1] = '\0';
    score.score = c.camera.x;
    save_score(score);

    // Cleanup resources
    liberer_animation(&myanimation);
    if(c.menu1) SDL_FreeSurface(c.menu1);
    if(c1.menu2) SDL_FreeSurface(c1.menu2);
    if(c2.menu3) SDL_FreeSurface(c2.menu3);
    if(button_surface) SDL_FreeSurface(button_surface);
    if(guide_surface) SDL_FreeSurface(guide_surface);
    if(font) TTF_CloseFont(font);
    if(button_font) TTF_CloseFont(button_font);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
