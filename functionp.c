#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "structp.h"
#include <stdbool.h>

#define NUM_FRAMES 2
#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 800

void afficher_background(SDL_Surface *screen, background c) {
    if(!screen || !c.menu1) return;  // Check for NULL surfaces
    SDL_BlitSurface(c.menu1, &c.camera, screen, NULL);
}
void initbackground(background* c) {
    c->menu1 = IMG_Load("back.jpg");
    c->camera.x = 0;
    c->camera.y = 250;
    c->camera.h = 800;
    c->camera.w = 1500;
}

void scrolling_right(background* c) {
    if(!c->shift_pressed) c->camera.x += 5;
    if(c->shift_pressed) c->camera.x += 5 * (int)c->acceleration;
    if(c->camera.x >= 7500) c->camera.x = 7500;
}

void scrolling_left(background* c) {
    if(!c->shift_pressed) c->camera.x -= 5;
    if(c->shift_pressed) c->camera.x -= 5 * (int)c->acceleration;
    if(c->camera.x <= 10) c->camera.x = 0;
}

void scrollingdown(background* c) {
    c->camera.y += 40;
    if(c->camera.y >= 270) c->camera.y = 270;
}

void scrollingup(background* c) {
    c->camera.y -= 40;
    if(c->camera.y <= 10) c->camera.y = 10;
}

void musique(int stage) {
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("Mix_OpenAudio error: %s\n", Mix_GetError());
        return;
    }
    
    char nom_musique[40];
    snprintf(nom_musique, sizeof(nom_musique), "musiques/%d.mp3", stage);
    
    Mix_Music *music = Mix_LoadMUS(nom_musique);
    if(!music) {
        printf("Mix_LoadMUS error: %s\n", Mix_GetError());
        return;
    }
    
    if(Mix_PlayMusic(music, -1) == -1) {
        printf("Mix_PlayMusic error: %s\n", Mix_GetError());
    }
    
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);  // Set to half volume
}
void save_score(Score score) {
    FILE *fp = fopen("score.txt", "a");
    if(fp) {
        fprintf(fp, "%s %d\n", score.name, score.score);
        fclose(fp);
    }
}

void show_top_scores(SDL_Surface *screen) {
    TTF_Font *font = TTF_OpenFont("font.ttf", 28);
    if(!font) return;

    Score scores[100] = {0};
    FILE *fp = fopen("score.txt", "r");
    if(fp) {
        for(int i = 0; i < 100; i++) {
            if(fscanf(fp, "%s %d", scores[i].name, &scores[i].score) == EOF) break;
        }
        fclose(fp);
    }

    for(int i = 0; i < 99; i++) {
        for(int j = i+1; j < 100; j++) {
            if(scores[j].score > scores[i].score) {
                Score temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    // Fix: Initialize all 4 components of SDL_Color
    SDL_Color textColor = {255, 255, 255, 255};  // Added alpha value (255 = fully opaque)
    
    for(int i = 0; i < 3; i++) {
        char scoreStr[50];
        sprintf(scoreStr, "%d. %s: %d", i+1, scores[i].name, scores[i].score);
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreStr, textColor);
        if(textSurface) {
            SDL_Rect textPos = {(screen->w - textSurface->w)/2, 50 + i*50, 0, 0};
            SDL_BlitSurface(textSurface, NULL, screen, &textPos);
            SDL_FreeSurface(textSurface);
        }
    }
    SDL_Flip(screen);
    SDL_Delay(2000);
    TTF_CloseFont(font);
}
void afficherTemps(SDL_Surface *ecran, TTF_Font *font, Uint32 tempsEcoule) {
    if(!ecran || !font) return;

    char tempsTexte[20];
    int minutes = tempsEcoule / 60;
    int secondes = tempsEcoule % 60;
    sprintf(tempsTexte, "Temps: %02d:%02d", minutes, secondes);

    // Initialize all 4 components
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Surface* surfaceTemps = TTF_RenderText_Solid(font, tempsTexte, blanc);
    if(surfaceTemps) {
        SDL_Rect position = {(ecran->w - surfaceTemps->w)/2, 10, 0, 0};
        SDL_BlitSurface(surfaceTemps, NULL, ecran, &position);
        SDL_FreeSurface(surfaceTemps);
    }
}

void initcamera(background1* c) {
    c->camera1.x = 0;
    c->camera1.y = 250;
    c->camera1.h = 800;
    c->camera1.w = 750;
}

void initcameramultiplayer(background1* c) {
    c->camera1.x = 0;
    c->camera1.y = 250;
    c->camera1.h = 800;
    c->camera1.w = 750;
}

void scrolling_right1(background1* c) {
    if(!c->shift_pressed) c->camera1.x += 5;
    if(c->shift_pressed) c->camera1.x += 5 * (int)c->acceleration;
    if(c->camera1.x >= 7500) c->camera1.x = 7500;
}

void scrolling_left1(background1* c) {
    if(!c->shift_pressed) c->camera1.x -= 5;
    if(c->shift_pressed) c->camera1.x -= 5 * (int)c->acceleration;
    if(c->camera1.x <= 10) c->camera1.x = 0;
}

void scrollingdown1(background1* c) {
    c->camera1.y += 40;
    if(c->camera1.y >= 600) c->camera1.y = 600;
}

void scrollingup1(background1* c) {
    c->camera1.y -= 40;
    if(c->camera1.y <= 10) c->camera1.y = 10;
}

int init_animation(Animation* anim) {
    if(!anim) return 0;  // Check for NULL pointer
    
    char filename[50];
    for(int i = 0; i < NUM_FRAMES; i++) {
        anim->anim[i] = NULL;  // Initialize to NULL
        sprintf(filename, "animation/frame%03d.png", i);
        anim->anim[i] = IMG_Load(filename);
        if(!anim->anim[i]) {
            printf("Erreur chargement frame %d: %s\n", i, SDL_GetError());
            // Free any already loaded frames
            for(int j = 0; j < i; j++) {
                if(anim->anim[j]) SDL_FreeSurface(anim->anim[j]);
            }
            return 0;
        }
    }
    return 1;
}

void afficher_animation(Animation anim, SDL_Surface* screen) {
    static int i = 0;
    if(anim.anim[i] && screen) {  // Check for NULL surfaces
        SDL_BlitSurface(anim.anim[i], NULL, screen, &anim.pos);
    }
    SDL_Delay(50);
    i = (i + 1) % NUM_FRAMES;
}

void liberer_animation(Animation* anim) {
    if(!anim) return;
    
    for(int i = 0; i < NUM_FRAMES; i++) {
        if(anim->anim[i]) {
            SDL_FreeSurface(anim->anim[i]);
            anim->anim[i] = NULL;
        }
    }
}


void draw_text(SDL_Surface *screen, const char *text, int x, int y, ColorRGBA color) {
    TTF_Font* font = TTF_OpenFont("font.ttf", 30);
if(!font) {
    printf("Erreur chargement police: %s\n", TTF_GetError());
    exit(1);
}
    // Initialize all 4 components
    SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, sdl_color);
    if(text_surface) {
        SDL_Rect pos = {x, y, 0, 0};
        SDL_BlitSurface(text_surface, NULL, screen, &pos);
        SDL_FreeSurface(text_surface);
    }
    TTF_CloseFont(font);
}

void show_guide(SDL_Surface *screen) {
    SDL_Surface *saved = SDL_ConvertSurface(screen, screen->format, screen->flags);
    if(!saved) return;

    SDL_Surface *overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 32,
                                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(!overlay) {
        SDL_FreeSurface(saved);
        return;
    }

    Uint32 color = SDL_MapRGBA(overlay->format, 200, 150, 255, 180);
    SDL_FillRect(overlay, NULL, color);
    SDL_SetAlpha(overlay, SDL_SRCALPHA, 180);
    SDL_BlitSurface(overlay, NULL, screen, NULL);
    SDL_FreeSurface(overlay);

    const char *lines[] = {
        "Guide du Jeu",
        "Fleches: Deplacer",
        "Shift: Accelerer",
        "P: Mode 2 joueurs",
        "A: Mode 1 joueur",
        "Cliquez pour fermer"
    };

    for(int i = 0; i < 6; i++) {
        draw_text(screen, lines[i], 
                 (screen->w - strlen(lines[i])*12)/2, 
                 150 + i*40, 
                 (ColorRGBA){255,255,255,255});
    }

    SDL_Flip(screen);

    SDL_Event e;
    int quit = 0;
    while(!quit && SDL_WaitEvent(&e)) {
        if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_QUIT) quit = 1;
    }

    SDL_BlitSurface(saved, NULL, screen, NULL);
    SDL_FreeSurface(saved);
    SDL_Flip(screen);
}
