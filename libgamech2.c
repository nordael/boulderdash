
#include <stdio.h>
#include <stdio.h>
#include "libgamech2.h"



/*************GAME SUPPORT FUNCTIONS *********************/

static char *load_map(){

    char* map_code;
    FILE * imap;

    map_code = malloc( sizeof(char)*(MAP_HIGHT*MAP_WIDTH) );

    if( map_code == NULL )
        fprintf( stderr, "FALHA AO ALOCAR MEMORIA"), exit(1);

    imap = fopen("./resources/cave.bdm", "r");
    
    if( imap == NULL )
        fprintf( stderr, "FALHA AO CARREGAR MAPA"), exit(1);
    
    fread( map_code, sizeof(char), 2, imap );

    if( map_code[0] != 'B' || map_code[1] != '-')
        fprintf( stderr, "ARQUIVO NAO SUPORTADO"), exit(1);
        
    fread( map_code, sizeof(char), (MAP_HIGHT*MAP_WIDTH), imap );
    
    fclose(imap);

    return map_code;

}

//untested
static ALLEGRO_BITMAP* sprite_cut( ALLEGRO_BITMAP *this_sheet, int x, int y, int w, int h ){
    
    ALLEGRO_BITMAP *new_sprite;

    new_sprite = al_create_sub_bitmap( this_sheet, x, y, w, h );
    if( !new_sprite )
        fprintf(stderr, "ERRO AO RECORTAR SPRITES"), exit(1);

    return new_sprite;
}

//untested
static void load_sprites( sprites_t *game_sprites ){
    
    int i, s;

    game_sprites->sprite_sheet = al_load_bitmap("./resources/sprites.png");
    if( !game_sprites->sprite_sheet )
        fprintf(stderr, "ERRO AO CARREGAR ARQUIVO DE SPRITES"), exit(1);

    s = SPRITE_SIZE;

    game_sprites->door[0] = sprite_cut( game_sprites->sprite_sheet, 2*s, 6*s , s, s);
    game_sprites->door[1] = sprite_cut( game_sprites->sprite_sheet, 2*s, 7*s , s, s);
    
    for( i = 0; i < 4; i++)    
        game_sprites->mv_rockford_dying[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 0, s, s );

    for( i = 0; i < 8; i++){
        game_sprites->mv_rockford_up[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 2*s, s, s );
        game_sprites->mv_rockford_left[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 2*s, s, s );
        game_sprites->mv_rockford_down[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 5*s, s, s );
        game_sprites->mv_rockford_right[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 5*s, s, s );        
        game_sprites->gem[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 10*s, s, s );
    }

    for( i = 0; i < 24; i++){
        game_sprites->mv_rockford_wait[i] = sprite_cut( game_sprites->sprite_sheet, (i%8)*s, (i/8 + 1)*s, s, s );
    }

    //how to read yellow/white num? they are half SPRITE_SIZE
    for( i = 0; i < 10; i++ ){
        game_sprites->white_num[i] = sprite_cut( game_sprites->sprite_sheet, 8*s, 8*s+i*(s/2), s, s/2 );
        game_sprites->yellow_num[i] = sprite_cut( game_sprites->sprite_sheet, 9*s, 8*s+i*(s/2), s, s/2 );
    }


    game_sprites->boulder = sprite_cut( game_sprites->sprite_sheet, 0, 7*s , s, s);
    game_sprites->dig_wall = sprite_cut( game_sprites->sprite_sheet, s, 7*s , s, s);
    game_sprites->iron_wall = sprite_cut( game_sprites->sprite_sheet, s, 6*s , s, s);
    game_sprites->stone_wall = sprite_cut( game_sprites->sprite_sheet, 3*s, 6*s , s, s);
    game_sprites->white_gem = sprite_cut( game_sprites->sprite_sheet, 8*s, 2*s , s, s/2);
    game_sprites->yellow_gem = sprite_cut( game_sprites->sprite_sheet, 9*s, 2*s , s, s/2);
  

}

static void unload_sprites( sprites_t *game_sprites ){
    
    int i;
    
    for( i = 0; i < 4; i++)    
        al_destroy_bitmap( game_sprites->mv_rockford_dying[i] );

    for( i = 0; i < 8; i++){
        al_destroy_bitmap( game_sprites->mv_rockford_up[i] );
        al_destroy_bitmap( game_sprites->mv_rockford_down[i] );
        al_destroy_bitmap( game_sprites->mv_rockford_left[i] );
        al_destroy_bitmap( game_sprites->mv_rockford_right[i] );
        al_destroy_bitmap( game_sprites->gem[i] );
    }

    for( i = 0; i < 24; i++ )
        al_destroy_bitmap( game_sprites->mv_rockford_wait[i] );

    //how to read yellow/white num? they are half SPRITE_SIZE
    for( i = 0; i < 10; i++ ){
        al_destroy_bitmap( game_sprites->white_num[i] );
        al_destroy_bitmap( game_sprites->yellow_num[i] );
    }

    al_destroy_bitmap( game_sprites->door[1] );
    al_destroy_bitmap( game_sprites->door[0] );

    al_destroy_bitmap( game_sprites->boulder );
    al_destroy_bitmap( game_sprites->dig_wall );
    al_destroy_bitmap( game_sprites->iron_wall );
    al_destroy_bitmap( game_sprites->stone_wall );
    al_destroy_bitmap( game_sprites->white_gem ); 
    al_destroy_bitmap( game_sprites->yellow_gem );
  
    al_destroy_bitmap( game_sprites->sprite_sheet );
}

static void create_game_map( game_t *game ){
    
    int i,w,r,g;
    char c[1], *map_code = NULL;
    int length, x, y;

    map_code = load_map();

    length = (MAP_HIGHT*MAP_WIDTH);
    w = 0; r = 0; g = 0;
    
    for( i = 0; i < length; i++ ){
        x = (i%MAP_WIDTH)*SPRITE_SIZE;
        y = (i/MAP_WIDTH)*SPRITE_SIZE + SPRITE_SIZE;

        c[0] = map_code[i], c[1] ='\0';
        switch( atoi( c ) ){
            case DIGGABLE:
                init_wall( &game->wall[w++], DIGGABLE );
                set_wall_position( (game->wall+w), x, y );
                break;
            case STONE:
                init_wall( &game->wall[w++], STONE );
                set_wall_position( (game->wall+w), x, y );
                break;
            case IRON:
                init_wall( &game->wall[w++], IRON );
                set_wall_position( (game->wall+w), x, y );
                break;
            case BOULDER:
                init_rock( &game->rock[r++] );
                set_rock_position( (game->rock+r), x, y );
                break; 
            case GEM:
                init_gem( &game->gem[g++] );
                set_gem_position( (game->gem+g), x, y );
                break;
            case DOLL:
                init_doll( &game->doll );
                set_doll_position( &game->doll, x, y );
                break;
            case DOOR:
                init_door( &game->exit );
                set_door_position( &game->exit, x, y );
            default: 
                break;
        }
    }
    free (map_code);
}

//unfinished
static game_t *create_game(){
    
    game_t *bdgame;

    bdgame = malloc( sizeof( game_t ) );
    if( !bdgame )
        fprintf( stderr, "ERRO AO ALOCAR MEMORIA"), exit(1);

    //initialize the keyboard map for game control
    memset( bdgame->key, 0, sizeof( bdgame->key ) );
   
    create_game_map( bdgame );    
    load_sprites( &bdgame->bd_sprites );
    bdgame->score = 0;
    bdgame->frames = 0;
    bdgame->clock = 150;
    bdgame->goal = 8*NGEMS/10;
    bdgame->total_gems = NGEMS;
    
    return bdgame;
}


/************* GAME CHECKING FUNCTIONS  ********************/

static int has_found_wall( rockford_t doll, wall_t *wall ){
    int i, s;
    s = SPRITE_SIZE;

    for( i = 0; i < NWALLS; i++ ){
        if ( has_collision( doll.x, doll.y, wall[i].x, wall[i].y, s, s ) ){
            switch ( wall[i].etype ){
                case IRON:
                case STONE: return 1; break;
                case DIGGABLE: return 0; break;
                default: break;
            }
        }
    }
    
    return 0;

}

static int should_doll_move( game_t *game, rockford_t doll, int step_x, int step_y ){

    rockford_t dummy;

    dummy.x = doll.x + step_x;
    dummy.y = doll.y + step_y;

    if( !has_found_wall( dummy, game->wall ) ) return 1;

    return 0;

}

static int doll_found_gem(  rockford_t doll, gem_t *gem ){
    int i, s;
    s = SPRITE_SIZE;

    if( gem->collect == FALSE )
        return 0;

    for( i = 0; i < NGEMS; i++ ){
        if ( has_collision( doll.x, doll.y, gem[i].x, gem[i].y, s, s ) ){
            gem[i].collect = FALSE;
            return 1;
        }
    }

    return 0;
}

/************** GAME UPDATE STATE FUNCTIONS **************/


static void refresh_keyboard( ALLEGRO_EVENT *event, game_t *game ){
    const int KEY_PRESSED = 1;
    const int KEY_RELEASED = 2;

    int i;

    switch( event->type ){ 

        case ALLEGRO_EVENT_TIMER:        

            for( i = 0; i < ALLEGRO_KEY_MAX; i++ )
                game->key[i] &= KEY_PRESSED;

            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            game->key[ event->keyboard.keycode] = KEY_PRESSED | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            game->key[ event->keyboard.keycode] &= KEY_RELEASED;
            break;
    }
}

//check for collision with rock and try move it
static void move_with_rock( rockford_t *doll, rock_t *rock, int step ){
    rockford_t dummy_d;
    rock_t *dummy_b;
    int i, s;


    dummy_d.x = doll->x + step;
    dummy_d.y = doll->y;
    s = SPRITE_SIZE;
    
    //if rockford finds a boulder, try grab it
    for( i = 0; i < NROCKS; i++ )
        if( has_collision( dummy_d.x, dummy_d.y, rock[i].x, rock[i].y, s, s ) )
            doll->grab = TRUE, dummy_b = (rock+i); 

    //check if a grabbed boulder collide with another. If it collides, do nothing
    for( i = 0; i < NROCKS; i++ ){
        if( has_collision( dummy_b->x+step, dummy_b->y, rock[i].x, rock[i].y, s, s ) ){
            doll->grab = FALSE;
            return;
        }
        else 
            break;
    }

    doll->x += step;
    dummy_b->x += step;
}

//not tested
static void update_score( game_t * game ){
    int s;

    s = SPRITE_SIZE;
    
    if( game->doll.gems && game->exit.open == FALSE )
        game->score = game->doll.gems*75;

    if ( game->clock && has_collision( game->doll.x, game->doll.y, game->exit.x, game->exit.y, s, s ) ){
        game->doll.moving = FALSE;
        game->score = game->clock*100;
        game->clock = 0;
    }
}

static void update_timer( game_t *game ){

    if( game->doll.moving == FALSE && game->exit.open == TRUE )
        return;

    if( !game->clock ){
        game->doll.alive = FALSE; 
        return;
    }

    //this must not count every 1/20 time tic
    game->clock--;
}

//not tested
static void update_doll( game_t *game  ){

    if(game->doll.alive == FALSE) return;

    if( game->doll.moving == FALSE && game->exit.open == TRUE ) return;

    //if( game->doll.respawn )
    //  game->doll.respawn--, return;

    if( game->key[ALLEGRO_KEY_G] ){

        if( game->key[ALLEGRO_KEY_LEFT] )
            game->doll.left_to_right = -1, move_with_rock( &game->doll, game->rock, -SPRITE_SIZE  );
        if( game->key[ALLEGRO_KEY_RIGHT])
            game->doll.left_to_right = 1, move_with_rock( &game->doll, game->rock, -SPRITE_SIZE  );
        
    }else{
        
        if( game->key[ALLEGRO_KEY_UP] || game->key[ALLEGRO_KEY_I] )
            game->doll.up_to_down = 1,
            game->doll.y -= ( should_doll_move( game, game->doll, 0, -SPRITE_SIZE ) ) ? SPRITE_SIZE :0 ;

        if( game->key[ALLEGRO_KEY_DOWN] || game->key[ALLEGRO_KEY_K] )
            game->doll.up_to_down = 1,
            game->doll.y+= ( should_doll_move( game, game->doll, 0, SPRITE_SIZE ) ) ? SPRITE_SIZE :0 ;

        if( game->key[ALLEGRO_KEY_LEFT] || game->key[ALLEGRO_KEY_L] )
            game->doll.left_to_right = -1, 
            game->doll.x-= ( should_doll_move( game, game->doll, -SPRITE_SIZE, 0 ) ) ? SPRITE_SIZE :0 ;

        if( game->key[ALLEGRO_KEY_RIGHT] || game->key[ALLEGRO_KEY_J] )
            game->doll.left_to_right = 1,
            game->doll.x+= ( should_doll_move( game, game->doll, SPRITE_SIZE, 0 ) ) ? SPRITE_SIZE :0 ;
    }
    
    //can't pass through iron wall
    if( game->exit.open == TRUE ){
        if( game->doll.x < SPRITE_SIZE )        
            game->doll.x = SPRITE_SIZE;             
        else if( game->doll.y < SPRITE_SIZE )
            game->doll.y = SPRITE_SIZE;           //2*SPRITE_SIZE;
        
        if( game->doll.x > MAP_WIDTH*SPRITE_SIZE )
            game->doll.x = MAP_WIDTH*SPRITE_SIZE;     //(MAP_WIDTH+1)*SPRITE_SIZE;
        else if( game->doll.y > MAP_HIGHT*SPRITE_SIZE )
            game->doll.y = MAP_HIGHT*SPRITE_SIZE;     //(MAP_HIGHT-1)*SPRITE_SIZE;
        
    }

    if( doll_found_gem( game->doll, game->gem ) ) //check for collision with a gem and update it
        game->doll.gems++;

}

static void update_wall( game_t* game ){
    int i, s;

    //if( game->doll.moving == FALSE && game->exit.open == TRUE ) return;

    s = SPRITE_SIZE;

    for( i = 0; i < NWALLS; i++ ){
        if( game->wall[i].etype == DIGGABLE && 
            game->wall[i].destroyed == FALSE &&
            has_collision( game->doll.x, game->doll.y, game->wall[i].x, game->wall[i].y, s, s ) )
                    game->wall[i].destroyed = TRUE;
    }
}

//untested
static void update_door( game_t * game ){

    if( game->doll.gems >= game->goal )
        game->exit.open = TRUE;
}

static void update_gems( game_t *game ){
    int i, j, s;

    if( game->doll.moving == FALSE && game->exit.open == TRUE ) return;

    s = SPRITE_SIZE;

   for( i = 0; i < NWALLS; i++ ){
       for( j = 0; j < NGEMS; j++ ){

            if( game->gem[i].collect == FALSE )
                continue;

            if( !has_collision( game->gem[i].x, game->gem[i].y+s, game->wall[i].x, game->wall[i].y, s, s ) ){
                game->gem[i].moving = TRUE; 
                game->gem[i].y += s;
            }

            if( has_collision( game->gem[i].x, game->gem[i].y+s, game->doll.x, game->doll.y, s, s ) &&
                               game->gem[i].moving == TRUE ){
                game->doll.gems++; 
                game->gem[i].y += s; 
                game->gem[i].collect = FALSE;
            }

            if( game->wall[i].etype == DIGGABLE || game->wall[i].etype == STONE )
                game->rock[i].moving = FALSE;                
        
            //if I call recursively by moving to left ou right, what would be the stop condition?
        }
   }

}

static void update_boulder( game_t *game ){
   int i, j, s;

    if( game->doll.moving == FALSE && game->exit.open == TRUE )return;

    s = SPRITE_SIZE;

   for( i = 0; i < NWALLS; i++ ){
       for( j = 0; j < NROCKS; j++ ){
            if( !has_collision( game->rock[i].x, game->rock[i].y+s, game->wall[i].x, game->wall[i].y, s, s ) ){
                game->rock[i].moving = TRUE;
                game->rock[i].y += s;
            } else{
                if( has_collision( game->rock[i].x, game->rock[i].y+s, game->doll.x, game->doll.y, s, s ) ){
                    game->doll.alive = FALSE;
                    game->rock[i].y += s;
                }

                if( game->wall[i].etype == DIGGABLE || game->wall[i].etype == STONE )
                    game->rock[i].moving = FALSE;                
            }
            //if I call recursively by moving to left ou right, what would be the stop condition?
        }
   }

}

/************** GAME DRAWING FUNCTIONS **************/

//draw the score on status bar
static void draw_score( game_t *game ){
    long digit;
    int x, y, i;

    digit = game->score;
    i = 0; y = 16; x = (MAP_WIDTH-3)*SPRITE_SIZE;   
    
    while( i < 7 ){
        al_draw_bitmap( game->bd_sprites.white_num[digit%10], x, y ,0 );
        x -= SPRITE_SIZE;
        digit /= 10 ;
        i++;
    }    

}

//draw the clock on status bar
static void draw_clock( game_t *game ){

    int digit;
    int x, y, i;

    digit = game->clock;
    i = 0; y = 16; x = (MAP_WIDTH-13)*SPRITE_SIZE;   
    
    while( i < 3 ){
        al_draw_bitmap( game->bd_sprites.white_num[digit%10], x, y ,0 );
        x -= SPRITE_SIZE;
        digit /= 10 ;
        i++;
    }    

}

static void draw_gems_collected( game_t *game ){
    int digit, y;

    digit = game->doll.gems;
    y = SPRITE_SIZE/2;

    al_draw_bitmap( game->bd_sprites.white_gem, 6*SPRITE_SIZE, y, 0 );
    al_draw_bitmap( game->bd_sprites.white_num[digit%10], 8*SPRITE_SIZE, y, 0 );
    digit /= 10;
    al_draw_bitmap( game->bd_sprites.white_num[digit%10], 7*SPRITE_SIZE, y, 0 );

}

//draw the total gems to be collected
static void draw_gems_goal( game_t *game ){
    int digit, y;

    digit = game->goal;
    y = SPRITE_SIZE/2;

    al_draw_bitmap( game->bd_sprites.yellow_gem, 3*SPRITE_SIZE, y, 0 );
    al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], 5*SPRITE_SIZE, y, 0 );
    digit /= 10;
    al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], 4*SPRITE_SIZE, y, 0 );
    
}

static void draw_wall( game_t *game ){
    int i;
    
    for( i = 0; i < NWALLS; i++ ){
        if( game->wall[i].destroyed == TRUE )
            al_draw_bitmap( game->bd_sprites.empty, game->wall[i].x, game->wall[i].y, 0);
        else{
            switch( game->wall[i].etype ){
                case DIGGABLE:
                    al_draw_bitmap( game->bd_sprites.dig_wall, game->wall[i].x, game->wall[i].y, 0);
                    break;
                case IRON:
                    al_draw_bitmap( game->bd_sprites.iron_wall, game->wall[i].x, game->wall[i].y, 0);
                    break;
                case STONE:
                    al_draw_bitmap( game->bd_sprites.stone_wall, game->wall[i].x, game->wall[i].y, 0);
                    break;
                default: break;
            }
        }
    }
}

static void draw_boulder( game_t *game ){
    int i;
    
    for( i = 0; i < NROCKS; i++ ){
        al_draw_bitmap( game->bd_sprites.boulder, game->rock[i].x, game->rock[i].y, 0);
    }
}

static void draw_door( game_t *game ){
    int frame, c = 2;
   
    if( game->exit.frames < 0 )
        game->exit.frames = 0;

    frame = game->exit.frames++;
    
    al_draw_bitmap( game->bd_sprites.door[frame%c], game->exit.x, game->exit.y, 0 );
}

static void draw_gems( game_t *game ){
    int i, frame, c;

    c = 8;                  //total of gems frame

    for( i = 0; i < NGEMS; i++ ){

        if( game->gem[i].frames < 0 )
            game->gem[i].frames = 0;

        frame = game->gem[i].frames++;

        al_draw_bitmap( game->bd_sprites.gem[ ( frame % c ) ], 
                        game->gem[i].x, game->gem[i].y, 0);
        
    }

}

static void draw_doll( game_t *game ){
    int wait, up_right, left_down;
    int frame;

    wait = 24;
    up_right = left_down = 8;

    if( game->doll.frames < 0 ) 
        game->doll.frames = 0;

    frame = game->doll.frames++;

    if( !game->doll.up_to_down || !game->doll.left_to_right )
        al_draw_bitmap( game->bd_sprites.mv_rockford_wait[frame%wait], game->doll.x, game->doll.y, 0 );
    
    if( game->doll.up_to_down < 0 || game->doll.left_to_right < 0 )
        al_draw_bitmap( game->bd_sprites.mv_rockford_left[frame%up_right], game->doll.x, game->doll.y, 0 );
    
    if( game->doll.up_to_down > 0 || game->doll.left_to_right > 0 )
        al_draw_bitmap( game->bd_sprites.mv_rockford_right[frame%left_down], game->doll.x, game->doll.y, 0 );

}

static void bd_draw_map( game_t * game ){

    draw_score( game );
    draw_clock( game );
    draw_gems_goal( game );
    draw_gems_collected( game );
    draw_wall( game );
    /*draw_boulder( game );
    draw_door( game );
    draw_gems( game );
    draw_doll( game );
    */
}

/************** GAME STATE FUNCTIONS **************/


void ignition( gstate_t *state, game_t **game ){

    //game_t *game = (*this_game);

    if( !al_init() )
        fprintf(stderr, "ALLEGRO NAO INICIADO"), exit(1);
    if( !al_install_keyboard() )
        fprintf(stderr, "ALLEGRO KEYBOARD NAO INICIADO"), exit(1);
    if( !al_init_image_addon() )
        fprintf(stderr, "ALLEGRO IMAGE ADDON NAO INICIADO"), exit(1);
    if ( !al_init_primitives_addon() )
        fprintf(stderr, "ALLEGRO PRIMITIVES ADDON NAO INICIADO"), exit(1);

    (*game) = create_game();

    (*state) = BEGIN;
}

void start_game( gstate_t *state, game_t *game ){

    game->timer = al_create_timer(1.0 / 20.0);
    game->queue = al_create_event_queue();
    game->disp = al_create_display( MAP_WIDTH*SPRITE_SIZE, (MAP_HIGHT+1)*SPRITE_SIZE );
    
    al_register_event_source( game->queue, al_get_display_event_source( game->disp ) );
    al_register_event_source( game->queue, al_get_keyboard_event_source());
    al_register_event_source( game->queue, al_get_timer_event_source( game->timer ) );
    
    //smooth visualization on the display
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    *state = PLAYING;
}

void running_game( gstate_t *state, game_t *game ){

    ALLEGRO_EVENT event;
    
    al_flush_event_queue(game->queue);
    bd_draw_map(game);
    al_flip_display();
    
    al_start_timer( game->timer );

    for(;;){
        break;
        al_wait_for_event( game->queue, &event );
        switch( event.type ){ 
            case ALLEGRO_EVENT_TIMER:
                update_wall(game);
                update_boulder(game);
                update_gems(game);
                update_door(game);
                update_score(game);
                update_timer(game);
                update_doll(game);
                /*game status display*/
                draw_gems_goal(game);
                draw_gems_collected(game);
                draw_score(game);
                draw_clock(game);
                /*game stuff*/
                draw_door(game);
                draw_wall(game);
                draw_boulder(game);
                draw_gems(game);
                draw_doll(game);                
                
                if( game->key[ALLEGRO_KEY_ESCAPE] )
                    *state = GAMEOVER;

                break;                
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                *state = GAMEOVER;
                break;
        }
        refresh_keyboard( &event, game );
        al_flip_display();
        if( *state == GAMEOVER )
            break;
    }
    al_stop_timer( game->timer );
    if( *state == GAMEOVER )
        return;
    
    *state = PLAYING;
    //*state = ENDMATCH;
}

/*restart the initial state of every element of 
 *the game and wait a time for next match to restart
 */
void ending_match( gstate_t *state, game_t *game ){
    int i;

    //should save the score and show it later

    game->clock = 0;
    game->goal = NGEMS;
    //game->total_gems
    //game->frame

    init_doll( &game->doll );
    init_door( &game->exit );
    
    for( i = 0; i < NWALLS; i++ )
        init_wall( &game->wall[i], game->wall[i].etype );

    for( i = 0; i < NGEMS; i++ )
        init_gem( &game->gem[i] );

    for( i = 0; i < NROCKS; i++ )
        init_rock( &game->rock[i] );

    *state = PLAYING;
}

//stop game: dealloc memory, turn off engine alegro
void end_game( game_t *game ){
    
    al_destroy_display( game->disp );
    al_destroy_timer( game->timer );
    al_destroy_event_queue( game->queue );
    
    unload_sprites( &game->bd_sprites );    
}