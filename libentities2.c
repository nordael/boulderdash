
#include "libentities2.h"
#include <stdio.h>
#include <stdlib.h>


void init_wall( wall_t *wall, entype_t this_type ){

    wall->x = wall->y = 0;
    wall->frames = 0;
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

void init_rock( rock_t *rock ){
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
    doll->dx = 0; 
    doll->dx =  0;
}

void init_door( door_t *door ){
    door->destructible = FALSE;
    door->open = FALSE;
    door->frames = 0;
    door->x = 0;
    door->y = 0;
}

//check for colision based on the sprite dimensions
int has_collision( int e1x, int e1y, int  e2x, int  e2y, int w, int h ){

    if( e1x > (e2x+w) ) return 0;
    if( (e1x+w) < e2x ) return 0;
    if( e1y > (e2y+h) ) return 0;
    if( (e1y+h) < e2y ) return 0;

    return 1;
}


void set_wall_position( wall_t *wall, int x, int y ){
    wall->x = x;
    wall->y = y;
}

void set_rock_position( rock_t *rock, int x, int y ){
    rock->x = x;
    rock->y = y;
}

void set_gem_position( gem_t *gem, int x, int y ){
    gem->x = x;
    gem->y = y;
}

void set_doll_position( rockford_t *doll, int x, int y ){
    doll->x = x;
    doll->y = y;
}

void set_door_position( door_t *door, int x, int y ){
    door->x = x;
    door->y = y;
}