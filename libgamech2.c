
#include <stdio.h>
#include "libgamech2.h"


/*************GAME SUPPORT FUNCTIONS *********************/

//read the the file containing map
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

static long* load_scores(){
    const int MAX = 10;
    int i;
    long *best_scores;
    char magic[2];

    FILE * iscr;

    best_scores = malloc( sizeof(long)*MAX );

    if( best_scores == NULL )
        fprintf( stderr, "FALHA AO ALOCAR MEMORIA"), exit(1);
    
    memset( best_scores, 0, sizeof( long ) );

    iscr = fopen("./resources/scores.bds", "r");
    
    if( iscr == NULL )
        fprintf( stderr, "FALHA AO ABRIR ARQUIVO"), exit(1);
    
    fread( magic, sizeof(char), 2, iscr );

    if( magic[0] != 'B' || magic[1] != 'S')
        fprintf( stderr, "ARQUIVO CORROMPIDO OU INVALIDO"), exit(1);
        
    //read the score from file
    for( i = 0; i < MAX; i++ )
        fscanf( iscr, "%ld\n", &best_scores[i] );
    
    fclose(iscr);
    
    return best_scores;
}

static void store_scores( long *best_scores ){
    
    const int MAX = 10;

    char magic[2];
    FILE *oscr;
    int i,j, max;
    long tmp;

    oscr = fopen("./resources/scores.bds", "w");
    
    if( oscr == NULL )
        fprintf( stderr, "FALHA AO ABRIR ARQUIVO"), exit(1);

    //sort to save
    for( i = 0; i < MAX; i++ ){
        max = i;
        for( j = i+1; j < MAX; j++ ){
            if( best_scores[max] < best_scores[j] )
                max = j;
        }
        tmp = best_scores[i];
        best_scores[i] = best_scores[max];
        best_scores[max] = tmp;
    }

    magic[0]='B'; magic[1] = 'S';
    fwrite( magic, sizeof(char), 2, oscr );
    
    for( i = 0; i < MAX; i++ )
        fprintf( oscr, "%ld\n", best_scores[i] );

    fclose(oscr);
}

//cut the main sprite sheet to make the game sprites
static ALLEGRO_BITMAP* sprite_cut( ALLEGRO_BITMAP *this_sheet, int x, int y, int w, int h ){
    
    ALLEGRO_BITMAP *new_sprite;

    new_sprite = al_create_sub_bitmap( this_sheet, x, y, w, h );
    if( !new_sprite )
        fprintf(stderr, "ERRO AO RECORTAR SPRITES"), exit(1);

    return new_sprite;
}

//load sprites
static void load_sprites( sprites_t *game_sprites ){
    
    int i, s;

    game_sprites->sprite_sheet = al_load_bitmap("./resources/sprites.png");
    if( !game_sprites->sprite_sheet )
        fprintf(stderr, "ERRO AO CARREGAR ARQUIVO DE SPRITES"), exit(1);

    s = SPRITE_SIZE;

    game_sprites->door[0] = sprite_cut( game_sprites->sprite_sheet, s, 6*s , s, s);
    game_sprites->door[1] = sprite_cut( game_sprites->sprite_sheet, 2*s, 6*s , s, s);
    
    for( i = 0; i < 4; i++)    
        game_sprites->mv_rockford_dying[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 0, s, s );

    for( i = 0; i < 8; i++){
        game_sprites->mv_rockford_up[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 4*s, s, s );
        game_sprites->mv_rockford_left[i] = sprite_cut( game_sprites->sprite_sheet, i*s, 4*s, s, s );
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
    game_sprites->empty = sprite_cut( game_sprites->sprite_sheet, 0, 6*s , s, s);
    game_sprites->iron_wall = sprite_cut( game_sprites->sprite_sheet, s, 6*s , s, s);
    game_sprites->stone_wall = sprite_cut( game_sprites->sprite_sheet, 3*s, 6*s , s, s);
    game_sprites->white_gem = sprite_cut( game_sprites->sprite_sheet, 8*s, 2*s , s, s/2);
    game_sprites->yellow_gem = sprite_cut( game_sprites->sprite_sheet, 9*s, 2*s , s, s/2);
  

}

//dispose all sprites
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
    al_destroy_bitmap( game_sprites->empty );
    al_destroy_bitmap( game_sprites->stone_wall );
    al_destroy_bitmap( game_sprites->white_gem ); 
    al_destroy_bitmap( game_sprites->yellow_gem );
  
    al_destroy_bitmap( game_sprites->sprite_sheet );
}

//generate the elements of the map
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
                init_wall( &game->wall[w], DIGGABLE );
                set_wall_position( (game->wall+w), x, y );
                w++;
                break;
            case STONE:
                init_wall( &game->wall[w], STONE );
                set_wall_position( (game->wall+w), x, y );
                w++;
                break;
            case IRON:
                init_wall( &game->wall[w], IRON );
                set_wall_position( (game->wall+w), x, y );
                w++;
                break;
            case BOULDER:
                init_rock( &game->rock[r] );
                set_rock_position( (game->rock+r), x, y );
                r++;
                break; 
            case GEM:
                init_gem( &game->gem[g] );
                set_gem_position( (game->gem+g), x, y );
                g++;
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

//just build the game
static game_t *create_game(){
    
    game_t *game;

    game = malloc( sizeof( game_t ) );
    if( !game )
        fprintf( stderr, "ERRO AO ALOCAR MEMORIA"), exit(1);

    //initialize the keyboard map for game control
    memset( game->key, 0, sizeof( game->key ) );
   
    create_game_map( game );    
    load_sprites( &game->bd_sprites );
    game->best = load_scores();

    game->score = 0;
    game->frames = 0;
    game->clock = CLOCK_COUNT;
    game->goal = 8*NGEMS/10;
    game->total_gems = NGEMS;
    
    return game;
}


/************* GAME CHECKING FUNCTIONS  ********************/

//return an integer 1 if has found a solid wall, 0 otherwise
//if 0, change the status of the wall to destroyed
static int has_found_wall( rockford_t doll, wall_t *wall ){
    int i, s;
    s = SPRITE_SIZE;

    for( i = 0; i < NWALLS; i++ ){

        if ( has_collision( doll.x, doll.y, wall[i].x, wall[i].y, s, s ) ){

            switch ( wall[i].etype ){
                case IRON:
                case STONE: return 2; break;
                case DIGGABLE: 
                    if( wall[i].destroyed == FALSE ) return 1;
                    else return 0;
                    break;
                default: break;
            }
        }
    }
    
    return 0;
}

//check if has collided with a rock
static int has_found_rock( rockford_t doll, rock_t *rock ){
    int i, s;
    s = SPRITE_SIZE;

    for( i = 0; i < NROCKS; i++ ){
        if ( has_collision( doll.x, doll.y, rock[i].x, rock[i].y, s, s ) )
             return 1;
    }
    
    return 0;
}

//check if can move ahead (return 1) or not (return 0)
static int should_doll_move( game_t *game, rockford_t doll, int step_x, int step_y ){

    rockford_t dummy;
    int s;

    s = SPRITE_SIZE;
    dummy.x = doll.x + step_x;
    dummy.y = doll.y + step_y;

    if( has_found_wall( dummy, game->wall ) > 1 ) return 0; // found a not diggable wall
    if( has_found_rock( dummy, game->rock ) ) return 0;
    
    if( has_collision( dummy.x, dummy.y, game->exit.x, game->exit.y, s, s ) 
        && game->exit.open == FALSE ){
        return 0;
    }

    return 1;

}

//check if rockford found a gem and set it as found
static int doll_found_gem(  rockford_t doll, gem_t *gem ){
    int i, s;
    s = SPRITE_SIZE;

    for( i = 0; i < NGEMS; i++ ){

        if ( gem[i].collect == TRUE && has_collision( doll.x, doll.y, gem[i].x, gem[i].y, s, s ) ){
            gem[i].collect = FALSE;
            return 1;
        }
    }

    return 0;
}

/************** GAME UPDATE STATE FUNCTIONS **************/

//update the keyboard listener
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
static void move_with_rock( game_t *game, int step ){
    rockford_t dummy_d;
    rock_t *dummy_b;
    int i, s;

    s = SPRITE_SIZE;

    dummy_b = NULL;                         //the boulder address to be tested
    dummy_d.x = game->doll.x + step;        //ghost dool which walks and sees the path
    dummy_d.y = game->doll.y;
    
    
    //if rockford finds a boulder, try grab it
    for( i = 0; i < NROCKS; i++ )
        if( has_collision( dummy_d.x, dummy_d.y, game->rock[i].x, game->rock[i].y, s, s ) )
            game->doll.grab = TRUE, dummy_b = (game->rock+i); 

    if( dummy_b == NULL ){
       game->doll.grab = FALSE;
       return;
    }

    //get the position of the rock ahead
    dummy_d.x = dummy_b->x + step;
    dummy_d.y = dummy_b->y; 

    //hasn't found some wall
    if( !has_found_wall( dummy_d, game->wall ) &&
        !has_found_rock( dummy_d, game->rock ) &&
        !doll_found_gem( dummy_d, game->gem ) ) {

        game->doll.x += step;
        dummy_b->x += step;     //move the exactly rock grabbeds
    }
        
    game->doll.grab = FALSE;    
}

//refresh the score display
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

//decrease the time remaining
static void update_timer( game_t *game ){

    if( !game->clock ){
        game->doll.alive = FALSE; 
        return;
    }

    game->frames++;
    if( game->frames < 0 )
        game->frames = 0;

    //this must not count every 1/20 time tic
    if ( game->frames % FRAMERATIO == 0 )
        game->clock--;
}

//refresh the rockford status
static void update_doll( game_t *game  ){

    if(game->doll.alive == FALSE) return;


    //if( game->doll.respawn )
    //  game->doll.respawn--, return;

    game->doll.up_to_down = 0;
    game->doll.left_to_right = 0;

    if( game->key[ALLEGRO_KEY_G] ){

        if( game->key[ALLEGRO_KEY_LEFT] ){
            game->doll.left_to_right = -1; 
            move_with_rock( game, -SPRITE_SIZE  );
        }else if( game->key[ALLEGRO_KEY_RIGHT]){
            game->doll.left_to_right = 1;
            move_with_rock( game, SPRITE_SIZE  );
        }
        
    }else{
        
        game->doll.grab = FALSE;

        if( game->key[ALLEGRO_KEY_UP] || game->key[ALLEGRO_KEY_I] ){
            game->doll.up_to_down = -1; //change the sprite moviment
            if( should_doll_move( game, game->doll, 0, -SPRITE_SIZE ) )
                game->doll.y -= SPRITE_SIZE;

        }else if( game->key[ALLEGRO_KEY_DOWN] || game->key[ALLEGRO_KEY_K] ){
                game->doll.up_to_down = 1;
                if ( should_doll_move( game, game->doll, 0, SPRITE_SIZE ) )
                    game->doll.y += SPRITE_SIZE;

        } else if( game->key[ALLEGRO_KEY_LEFT] || game->key[ALLEGRO_KEY_J] ){
            game->doll.left_to_right = -1; //change the sprite moviment
            if ( should_doll_move( game, game->doll, -SPRITE_SIZE, 0 ) )
                game->doll.x -= SPRITE_SIZE;
                
        }else if( game->key[ALLEGRO_KEY_RIGHT] || game->key[ALLEGRO_KEY_L] ){
                game->doll.left_to_right = 1;
                if ( should_doll_move( game, game->doll, SPRITE_SIZE, 0 ) )
                    game->doll.x += SPRITE_SIZE;
        }
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

    //check for collision with a gem and update it
    if( doll_found_gem( game->doll, game->gem ) ) 
        game->doll.gems++;

}

//refresh the status of any wall
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

//change the door status open
static void update_door( game_t * game ){

    if( game->doll.gems >= game->goal )
        game->exit.open = TRUE;
    
}

//refressh gem status 
static void update_gems( game_t *game ){
    int i, j, s;
    int ABOVE_WALL, ABOVE_ROCK, ABOVE_GEM;
    gem_t dummy_gem;

    s = SPRITE_SIZE;
    
   for( j = 0; j < NGEMS; j++ ){
       dummy_gem.x = game->gem[j].x;
       dummy_gem.y = game->gem[j].y + s;

       if( game->gem[j].collect == FALSE )
            continue;
        
        ABOVE_WALL = 0; ABOVE_ROCK = 0; ABOVE_GEM = 0;

        for( i = 0; i < NWALLS; i++ ){
            if( has_collision( dummy_gem.x, dummy_gem.y, game->wall[i].x, game->wall[i].y, s, s ) ){
                if( game->wall[i].destroyed == FALSE ){
                    ABOVE_WALL = 1;
                    game->gem[j].moving = FALSE;
                    break;
                }
            }
        }

        if( !ABOVE_WALL ){

            for( i = 0; i < NROCKS; i++ ){
                if( has_collision( dummy_gem.x, dummy_gem.y, game->rock[i].x, game->rock[i].y, s, s ) ){
                    ABOVE_ROCK = 1;
                    game->gem[j].moving = FALSE;
                    break;
                }                 
            }
        }

        if( !ABOVE_ROCK && !ABOVE_WALL ){            
            for( i = 0; i < NGEMS; i++ ){
                if( has_collision( dummy_gem.x, dummy_gem.y, game->gem[i].x, game->gem[i].y, s, s ) ){
                    if( game->gem[i].collect == TRUE  ){
                        ABOVE_GEM = 1;
                        game->gem[j].moving = FALSE;
                        break;
                    }
                }                   
            }
        }

        if( !ABOVE_GEM && !ABOVE_ROCK && !ABOVE_WALL )
            game->gem[j].moving = TRUE;


        if( game->gem[j].moving == TRUE )
            game->gem[j].y += ( game->frames % 5 == 0 ) ? s :0;
    }

}

//refresh rock status
static void update_boulder( game_t *game ){
    int i, j, s;
    rock_t rdummy;
    int ABOVE_ROCK, ABOVE_GEM, ABOVE_WALL;


    s = SPRITE_SIZE;
  

   for( j = 0; j < NROCKS; j++ ){
       rdummy.x = game->rock[j].x;
       rdummy.y = game->rock[j].y + s;

       ABOVE_WALL = 0; ABOVE_ROCK = 0; ABOVE_GEM = 0;

       for( i = 0; i < NWALLS; i++ ){
            if( has_collision( rdummy.x, rdummy.y, game->wall[i].x, game->wall[i].y, s, s ) ){
                if( game->wall[i].destroyed == FALSE ){
                    ABOVE_WALL = 1;
                    game->rock[j].moving = FALSE;
                    break;
                }
            }
        }

        if( !ABOVE_WALL ){
            for( i = 0; i < NROCKS; i++ ){
                if( has_collision( rdummy.x, rdummy.y, game->rock[i].x, game->rock[i].y, s, s ) ){
                    ABOVE_ROCK = 1;
                    game->rock[j].moving = FALSE;
                    break;
                }
            }
        }

        if( !ABOVE_WALL && !ABOVE_ROCK ){
            for( i = 0; i < NGEMS; i++ ){
                if( has_collision( rdummy.x, rdummy.y, game->gem[i].x, game->gem[i].y, s, s ) ){
                    if( game->gem[i].collect == TRUE  ){
                        ABOVE_GEM = 1;
                        game->rock[j].moving = FALSE;
                        break;
                    }
                }
            }
        }

        if( !ABOVE_WALL && !ABOVE_GEM && !ABOVE_ROCK ){
            if( has_collision( rdummy.x, rdummy.y, game->doll.x, game->doll.y, s, s ) ){
                if( game->rock[j].moving == TRUE )
                    game->doll.alive = FALSE;
            }else{
                game->rock[j].moving = TRUE;
            }
        }

        if ( game->rock[j].moving == TRUE )
            game->rock[j].y += ( game->frames % 5 == 0 ) ? s : 0;
   }

}

//refresh all status map
static void bd_update_map( game_t *game ){

    if( game->doll.alive == FALSE ) return;

    update_score(game); 
    update_timer(game); 
    update_door(game);  
    update_wall(game);  
    update_gems(game);  
    update_doll(game);
    update_boulder(game);
    
}

/************** GAME DRAWING FUNCTIONS **************/

//draw the score on status bar
static void draw_score( game_t *game ){
    long digit;
    int x, y, i;

    digit = game->score;
    i = 0; 
    x = (MAP_WIDTH-3)*SPRITE_SIZE;
    y = SPRITE_SIZE/2;    
    
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

//draw the total gems collected by rockford
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

    digit = game->total_gems;
    y = SPRITE_SIZE/2;

    al_draw_bitmap( game->bd_sprites.yellow_gem, 3*SPRITE_SIZE, y, 0 );
    al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], 5*SPRITE_SIZE, y, 0 );
    digit /= 10;
    al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], 4*SPRITE_SIZE, y, 0 );
    
}

//draw all the walls 
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

//draw the killer boulders
static void draw_boulder( game_t *game ){
    int i;
    
    for( i = 0; i < NROCKS; i++ ){
        al_draw_bitmap( game->bd_sprites.boulder, game->rock[i].x, game->rock[i].y, 0);
    }
}

//draw the door, but firstly hidden
static void draw_door( game_t *game ){
    int frame, c = 2;
   
    if( game->exit.frames < 0 )
        game->exit.frames = 0;

    frame = (game->exit.frames++)/FRAMERATIO;
    
    if( game->exit.open == TRUE )
        al_draw_bitmap( game->bd_sprites.door[frame%c], game->exit.x, game->exit.y, 0 );
    else
        al_draw_bitmap( game->bd_sprites.door[0], game->exit.x, game->exit.y, 0 );
}

//draw the gems animated
static void draw_gems( game_t *game ){
    int i, frame, c;

    c = 8;                  //total of gems frame

    for( i = 0; i < NGEMS; i++ ){

        if( game->gem[i].frames < 0 )
            game->gem[i].frames = 0;

        frame = game->gem[i].frames++;

        if( game->gem[i].collect == TRUE )
            al_draw_bitmap( game->bd_sprites.gem[ ( frame % c ) ], 
                        game->gem[i].x, game->gem[i].y, 0);
        
    }

}

//draww the rockford dying
static void draw_doll_dying( game_t *game ){
    int frame, dying;

    if( game->doll.alive == TRUE ) return;

    if( game->doll.frames < 0 ) 
        game->doll.frames = 0;

    dying = 4;
    frame = game->doll.frames++;
    if( game->doll.frames < 0 ) game->doll.frames = 0;

    al_draw_bitmap( game->bd_sprites.mv_rockford_dying[frame%dying], game->doll.x, game->doll.y, 0 );

}

//draw the rockford animated
static void draw_doll( game_t *game ){
    int wait, up_right, left_down;
    int frame;

    if( game->doll.alive == FALSE ) return;

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

static void draw_doll_spawing( game_t *game ){

    int frame, c = 2; 

    frame = (game->doll.frames++)/FRAMERATIO;
    
    game->doll.respawn -= ( frame % 2 );

    if( game->doll.frames < 0 )
        game->doll.frames = 0;

    al_draw_bitmap( game->bd_sprites.door[frame%c], game->doll.x, game->doll.y, 0 );
}

static void show_best_scores( game_t *game ){

    ALLEGRO_FONT* font = al_create_builtin_font();

    int x, y, i, count;
    long digit;

    x = (MAP_WIDTH*SPRITE_SIZE)/3;
    y = (MAP_HIGHT*SPRITE_SIZE)/3;

    game->best[9] = game->score;
    
    i = 0; 

    al_draw_text(font, al_map_rgb(255, 255, 255), x, 4*SPRITE_SIZE, 0, "BESTS");

    //Draw the scores
    while( i < 10 ){
        digit = game->best[i];
        x = (MAP_WIDTH*SPRITE_SIZE)/3;
        count = 7; //maximum digits of score
        while( digit ){
            if( i == 9 )
                al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], x, y ,0 );
            else
                al_draw_bitmap( game->bd_sprites.white_num[digit%10], x, y ,0 );

            x -= SPRITE_SIZE;
            digit /= 10 ;
            count--;
        }
        
        while( count > 0 ){
            --count;
            if( i == 9 )
                al_draw_bitmap( game->bd_sprites.yellow_num[digit%10], x, y ,0 );
            else
                al_draw_bitmap( game->bd_sprites.white_num[digit%10], x, y ,0 );
            x -= SPRITE_SIZE;
        }

        y += SPRITE_SIZE;
        i++;
    }
}

//draw the elements of the map
static void bd_draw_map( game_t * game ){

    if( game->doll.alive == FALSE ){
        show_best_scores( game );
        return;
    }
    /*game status display*/
    draw_score( game );
    draw_clock( game );
    draw_gems_goal( game );
    draw_gems_collected( game );
    /*game stuff*/
    draw_wall( game );
    draw_boulder( game );
    draw_door( game );
    draw_gems( game );
    
    if( game->doll.respawn > 0 ){
        draw_doll_spawing( game );
    } else {
        draw_doll( game );
        draw_doll_dying( game );
    }
}


//show instructions of the game
static void show_instructions( game_t *game ){

    ALLEGRO_FONT* font = al_create_builtin_font();

    int x = (MAP_HIGHT*SPRITE_SIZE)/4;

    al_draw_text(font, al_map_rgb(255, 255, 255), x, 4*SPRITE_SIZE, 0, "Hello stranger!");
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 6*SPRITE_SIZE, 0, "Use as teclas de setas do seu PC ou as letras I, J, K, L para mover Rockford");
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 8*SPRITE_SIZE, 0, "Para mover uma rocha livre, use as teclas G e J ou G e L");
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 10*SPRITE_SIZE, 0, "Para reiniciar o jogo tecle ENTER. Para sair tecle ESC");
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 12*SPRITE_SIZE, 0, "Você tem um tempo restrito para pegar as gemas, use-o com sabedoria ou...");
    al_draw_text(font, al_map_rgb(255, 255, 255), x, 14*SPRITE_SIZE, 0, "Segure F1 ou H para abrir esta ajuda");

    al_draw_text(font, al_map_rgb(255, 255, 255), x, 16*SPRITE_SIZE, 16*SPRITE_SIZE, "JULIO L. MARTINS");
    
    al_flip_display();
    al_destroy_font(font);
}

/************** GAME STATE FUNCTIONS **************/

//starting essential addons from ALLEGRO 5 and create the game map
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
    if( !al_init_font_addon() )
        fprintf(stderr, "ALLEGRO FONT ADDON NAO INICIADO"), exit(1);

    (*game) = create_game();

    (*state) = BEGIN;
}

//intilizing some essential resources from ALLEGRO 5
void start_game( gstate_t *state, game_t *game ){

    game->timer = al_create_timer(1.0 / FRAMERATIO);
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

//the game itself
void running_game( gstate_t *state, game_t *game ){

    ALLEGRO_EVENT event;
    
    al_flush_event_queue(game->queue);  
    al_start_timer( game->timer );

    for(;;){
        
        al_wait_for_event( game->queue, &event );
        switch( event.type ){ 
            case ALLEGRO_EVENT_TIMER:                  

                //clears the background before drawings
                al_clear_to_color(al_map_rgb_f(0, 0, 0));
                
                if( game->key[ALLEGRO_KEY_H] || game->key[ALLEGRO_KEY_F1] ){
                    //show instructions
                    show_instructions( game );
                }else{                  
                    //all functions that update status
                    bd_update_map( game );
                    //all functions that draw sprites
                    bd_draw_map( game );
                    al_flip_display();
                }
                if( game->key[ALLEGRO_KEY_ESCAPE] )
                   *state = GAMEOVER;

                if( game->key[ALLEGRO_KEY_ENTER] )
                    *state = ENDMATCH;

                break;                
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                *state = GAMEOVER;
                break;
        }
        refresh_keyboard( &event, game );

        if( *state == GAMEOVER || *state == ENDMATCH )
            break;
    }
    al_stop_timer( game->timer );
    if( *state == GAMEOVER )
        return;
    
    *state = ENDMATCH;
}

/*restart the initial state of every element of 
 *the game and wait a time for next match to restart
 */
void ending_match( gstate_t *state, game_t *game ){

    game->clock = CLOCK_COUNT;
    game->goal = 8*NGEMS/10;
    game->total_gems = NGEMS;
    game->frames = 0;
    game->score = 0;

    store_scores( game->best );
    create_game_map(game);

    *state = PLAYING;
}

//stop game: dealloc memory, turn off engine alegro
void end_game( game_t *game ){
    //should save the score and show it later
    
    al_destroy_display( game->disp );
    al_destroy_timer( game->timer );
    al_destroy_event_queue( game->queue );
    
    unload_sprites( &game->bd_sprites );    
    free( game->best );
    free( game );
}