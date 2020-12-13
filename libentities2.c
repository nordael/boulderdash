
#include "libentities2.h"


void init_wall( wall_t *wall, entype_t this_type ){

    wall->x = 0;
    wall->y = 0;
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
    gem->x = 0;
    gem->y = 0;
    gem->dx = 0;
    gem->dy = 0;
}

void init_rock( rock_t *rock ){
    rock->moving = FALSE;
    rock->x = 0;
    rock->y = 0;
    rock->dx = 0;
    rock->dy = 0;
}

void init_doll( rockford_t *doll ){
    doll->gems = 0;
    doll->alive = TRUE;
    doll->moving = FALSE;
    doll->grab = FALSE;
    doll->respawn = 3;
    doll->frames = 0;
    doll->up_to_down = 0;
    doll->left_to_right = 0;
    doll->x = 0;
    doll->y = 0;
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

//check for colisions
int has_collision( int ex1, int ey1, int ex2, int ey2, int w, int h ){

    if( ex1 > (ex2+w-1) ) return 0;
    if( (ex1+w-1) < ex2 ) return 0;
    if( ey1 > (ey2+h-1) ) return 0;
    if( (ey1+h-1) < ey2 ) return 0;

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