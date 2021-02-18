// WARNING ULTRA DIRTY CODE - just my experimental playground
// playing around to revive my memory into basic linear physics implementation in code
// simple cube that accelerates by a "engine accel force"
// and that decelerates by air resistance
// while figuring out c++ in parallel

#include <iostream>
#include <algorithm>
#include <math.h>
#include <SDL2/SDL.h>

struct RigidBody
{
    SDL_Rect r = {
        .x = 800/2-20/2,
        .y = 20,
        .w = 20,
        .h = 20,
    }; 
    float pos_y = 20;
    float mass;
    float v_y=0;
    float a_y;
    float eng_y;
};

struct Clock
{
    uint32_t last_tick_time = 0;
    uint32_t delta = 0;
    float dT;

    void tick()
    {
        uint32_t tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
        dT = (float)delta/1000.0f;
        last_tick_time = tick_time;
    }
};

int main(int argv, char** args)
{
    SDL_Init(SDL_INIT_EVERYTHING);


    SDL_Window *window = SDL_CreateWindow("wing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool isRunning = true;
    SDL_Event event;
    

    RigidBody object1;
    object1.mass = 50.0f;
    int accel = 200;
    float g = 0;
    float time = 0;
    Clock mainClock;
    while (isRunning) {         
        while( SDL_PollEvent( &event ) ){
            /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
            switch( event.type ){
            case SDL_KEYDOWN:
                object1.eng_y+= 150.0f;
                accel = 200;
                printf( "Key press detected\n" );
                break;

            case SDL_KEYUP:
                object1.eng_y = 0;
                accel = 0;
                printf( "Key release detected\n" );
                break;

            default:
                break;
            }
        }

        // physics round
        mainClock.tick();
        SDL_Delay(50);

        // natural deceleration  should be due to friction + air resistance
        // friction & air-resistance is proportional to downforce. Downforce is proportional to speed square
        float resistance = 0.05f * object1.v_y * object1.v_y;
        object1.a_y = (object1.eng_y-resistance);
        object1.v_y += object1.a_y * mainClock.dT;

        object1.pos_y += object1.v_y * mainClock.dT;
        object1.r.y = object1.pos_y;
        std:: cout << object1.v_y << "\t" << object1.a_y << "\t" << object1.eng_y << "\t" << resistance << std::endl;

        time = time + mainClock.dT;

        if (object1.r.y > 600-20){
            break;
        }
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor( renderer, 0, 0, accel, 255 );
        SDL_RenderFillRect( renderer, &object1.r );
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}