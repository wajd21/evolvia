#ifndef STRUCTP_H
#define STRUCTP_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

typedef struct {
    SDL_Rect camera;
    int shift_pressed;
    SDL_Surface *menu1;
    int walk_right_permission;
    int walk_left_permission;
    int is_running_right;
    int is_running_left;
    float acceleration;
    Uint32 start_time;
    Uint32 elapsed_time;
} background;

typedef struct {
    SDL_Rect camera1;
    int shift_pressed;
    SDL_Surface *menu2;
    SDL_Surface *menu3;
    int walk_right_permission;
    int walk_left_permission;
    int is_running_right;
    int is_running_left;
    float acceleration;
    Uint32 start_time;
    Uint32 elapsed_time;
} background1;

typedef struct {
    char name[20];
    int score;
} Score;

typedef struct {
    SDL_Surface *anim[100];
    SDL_Rect pos;
} Animation;

typedef struct {
    Uint8 r, g, b, a;
} ColorRGBA;


void initbackground(background* c);
void afficher_background(SDL_Surface *screen, background c);
void scrolling_right(background* c);
void scrolling_left(background* c);
void scrollingup(background* c);
void musique(int stage);
void scrollingdown(background* c);
void save_score(Score score);
void show_top_scores(SDL_Surface *screen);
void afficherTemps(SDL_Surface *ecran, TTF_Font *font, Uint32 tempsEcoule);
void initcamera(background1* c);
void initcameramultiplayer(background1* c);
void scrolling_right1(background1* c);
void scrolling_left1(background1* c);
void scrollingup1(background1* c);
void scrollingdown1(background1* c);
void liberer_animation(Animation* anim);
int init_animation(Animation* anim);
void afficher_animation(Animation anim, SDL_Surface* screen);
void show_guide(SDL_Surface *screen);
void draw_text(SDL_Surface *screen, const char *text, int x, int y, ColorRGBA color);

#endif
