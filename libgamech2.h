#ifndef __LIBGAMECH2_H
#define __LIBGAMECH2_H

#include "libentities2.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>


#define MAP_WIDTH 40
#define MAP_HIGHT 22
#define SPRITE_SIZE 32
#define NWALLS 120+477+130
#define NGEMS 25
#define NROCKS 127
#define FRAMERATIO 20
#define CLOCK_COUNT 150


typedef enum {
    PRESET,     //set ALLEGRO engine and other game stuff 
    BEGIN,      //move to local game setup state
    PLAYING,    //the game
    ENDMATCH,   //reset game initial state
    GAMEOVER    //finish game
} gstate_t;

typedef struct bmps{
    ALLEGRO_BITMAP *sprite_sheet;
    ALLEGRO_BITMAP *boulder;
    ALLEGRO_BITMAP *iron_wall;
    ALLEGRO_BITMAP *stone_wall;
    ALLEGRO_BITMAP *dig_wall;
    ALLEGRO_BITMAP *empty;
    ALLEGRO_BITMAP *door[2];
    ALLEGRO_BITMAP *white_gem;            
    ALLEGRO_BITMAP *yellow_gem;            
    ALLEGRO_BITMAP *mv_rockford_dying[4];
    ALLEGRO_BITMAP *mv_rockford_wait[24];
    ALLEGRO_BITMAP *mv_rockford_up[8];
    ALLEGRO_BITMAP *mv_rockford_down[8];
    ALLEGRO_BITMAP *mv_rockford_left[8];
    ALLEGRO_BITMAP *mv_rockford_right[8];
    ALLEGRO_BITMAP *gem[8];
    ALLEGRO_BITMAP *yellow_num[10];
    ALLEGRO_BITMAP *white_num[10];
} sprites_t;

typedef struct game{
    long score;
    int clock;          //counts the time remaining
    int goal;           //the number of gems to be found
    int total_gems;
    int frames;
    rockford_t doll;
    rock_t rock[NROCKS];
    wall_t wall[NWALLS];
    gem_t gem[NGEMS];
    door_t exit;
    sprites_t bd_sprites;

    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_DISPLAY* disp;
    ALLEGRO_TIMER* timer;
    unsigned char key[ALLEGRO_KEY_MAX];

} game_t;


//starts the engine allegro
void ignition( gstate_t *state, game_t **game );

//builds the game's entities
void start_game( gstate_t *state, game_t *game );

//the game itself
void running_game( gstate_t *state, game_t *game );

//finish the current match
void ending_match( gstate_t *state, game_t *game );

//stop game
void end_game( game_t *game );


#endif