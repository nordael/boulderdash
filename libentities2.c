
#include "libentities2.h"
#include <stdio.h>
#include <stdlib.h>


void init_wall( wall_t *wall, entity_t this_type ){

    wall->x = wall->y = 0;
    frames = 0;
    wall->etype = this_type;

    switch(this_type){
        case DIGGABLE:
            wall->destructible = TRUE;
            wall->destroyed = FALSE;
            break;
        case STONE:
            wall->destructible = TRUE;
            wall->destroyed = FALSE;
            break;
        case IRON:
            wall->destructible = FALSE;
            wall->destroyed = FALSE;
            break;
        case MAGIC:
            wall->destructible = TRUE;
            wall->destroyed = FALSE;
            break;
        default:
            wall->destructible = FALSE;
            wall->destroyed = TRUE;
            break;
    }

}

void init_gem( gem_t *gem ){
    gem->moving = FALSE;
    gem->collect = TRUE;
    gem->frames = 0;
    gem->x = gem->y = 0;
    gem->dx = gem->dy = 0;
}

void init_rock( boulder_t *rock ){
    rock->moving = FALSE;
    rock->x = rock->y = 0;
    rock->dx = rock->dy = 0;
}

void init_doll( rockford_t *doll ){
    doll->gems = 0;
    doll->alive = TRUE;
    doll->moving = FALSE;
    doll->grab = FALSE;
    doll->respawn = 120;
    doll->frames = 0;
    doll->x = doll->y = 0;
    doll->dx = doll->dx =  0;
}

void init_doll( door_t *door ){
    door->destructible = FALSE;
    door->open = FALSE;
    door->frames = 0;
    door->x = door->y = 0;
}

//check for colision based on the sprite dimensions
int has_collision( int e1x, int e1y, int  e2x, int  e2y, int w, int h ){

    if( e1x > (e2x+w) ) return 0;
    if( (e1x+w) < e2x ) return 0;
    if( e1y > (e2y+h) ) return 0;
    if( (e1y+h) < e2y ) return 0;

    return 1;
}
