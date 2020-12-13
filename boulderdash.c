/*This is contain the main game loop*/


#include "libgamech2.h"


int main(){

    gstate_t gstate;
    game_t *bd_game = NULL;

    gstate = PRESET;

    for(;;){
        
        switch( gstate ){
            case PRESET: 
                ignition( &gstate, &bd_game );
                break;
            case BEGIN: 
                start_game( &gstate, bd_game );
                break;
            case PLAYING:
                running_game( &gstate, bd_game );
                break;
            case ENDMATCH:
                ending_match( &gstate, bd_game );
                break;
            case GAMEOVER:
                end_game( bd_game );
                break;
            default: break;
        }
        if(gstate == GAMEOVER)
            break;
    }

    return 0;
}