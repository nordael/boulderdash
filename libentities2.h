#ifndef __LIBENTITIES2_H
#define __LIBENTITIES2_H

typedef enum{ TRUE, FALSE } boolean_t;

//which kind of wall is it?
typedef enum { 
        EMPTY, 
        DIGGABLE, 
        STONE,
        MAGIC,
        IRON, 
        BOULDER, 
        GEM, 
        DOLL,
        DOOR
} entype_t;

//status of the wall
typedef enum {
        SOFT,                //can pass through it easily
        DESTROYED,           //diggable and stone walls can be destroyed
        BLINKING,            //magic wall blinks in a pattern
        TIGHTLYFIT,          //unmovable but destructible
        INDESTRUCTIBLE,      //unmovable and indestructible
        ERODED,             //can roll/fall and kill (only boulders)
        MOVABLE,            //rockford can move it
        COLLECTABLE,        //able to colect (diamonds)
} enstatus_t;

typedef struct door{
    boolean_t destructible; //if true can be destructed
    boolean_t open;         //if true, rockford can enter
    int frames;             
    int x, y; 
} door_t;

typedef struct wall{
    entype_t etype;         //which wall is it?
    boolean_t destructible; //if true can be destructed
    boolean_t destroyed;    //if true, replace for empty
    int frames;
    int x, y;               //current position
} wall_t;

typedef struct gem{
    boolean_t moving;
    boolean_t collect;   //if true, rockford can collect
    int frames;
    int x, y;           //current position
    int dx, dy;         //velocity
} gem_t;

typedef struct rock{
    boolean_t moving;   //true when it falls
    int x, y;           //current position
    int dx, dy;         //velocity
} rock_t;

typedef struct doll{
    int gems;               //diamonds collected. its zero if not the rockford
    boolean_t alive;
    boolean_t moving;       //if false, rockford has accomplished the goal
    boolean_t grab;         //if true, rockford can move boulder
    int up_to_down;         //-1 (up), 0 (waiting), 1 (down)
    int left_to_right;      //-1 (left), 0 (waiting), 1 (right)
    int respawn;
    int frames;
    int x, y;
    int dx, dy;
} rockford_t;



void init_wall( wall_t *wall, entype_t this_type );

void init_gem( gem_t *gem );

void init_rock( rock_t *rock );

void init_doll( rockford_t *doll );

void init_door( door_t *door );

int has_collision( int e1x, int e1y, int  e2x, int  e2y, int w, int h );

void set_wall_position( wall_t *wall, int x, int y );

void set_rock_position( rock_t *rock, int x, int y );

void set_gem_position( gem_t *gem, int x, int y );

void set_doll_position( rockford_t *doll, int x, int y );

void set_door_position( door_t *door, int x, int y );

#endif