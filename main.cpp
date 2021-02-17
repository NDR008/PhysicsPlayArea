#include <iostream>
#include <math.h>
#include <SDL2/SDL.h>

// just trying to create a body to give physics to
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
    /* data */
};

struct Clock
{
    uint32_t last_tick_time = 0;
    uint32_t delta = 0;

    void tick()
    {
        uint32_t tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
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
    float g = 0;
    Clock mainClock;
    while (isRunning)
    {         
        
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    isRunning = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        isRunning = false;
                    }
                    if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        g = 9.8/1000;
                    }
                    if (event.key.keysym.sym == SDLK_UP)
                    {
                        g = -9.8/1000;
                    }
            }
        }
        if (object1.r.y > 600){
                object1.pos_y = 0;
        }
        if (object1.r.y < 0){
                object1.pos_y = 600;
        }

        // physics round
        mainClock.tick();
        SDL_Delay(2000);
        float old_pos = object1.r.y;
        object1.pos_y = object1.pos_y + object1.v_y * mainClock.delta + (0.5f * g * mainClock.delta * mainClock.delta)/(1000.0f*1000.0f);
        object1.v_y = (object1.pos_y - old_pos)/mainClock.delta;
        object1.r.y = object1.pos_y;

        std::cout << "g " << g << " time :" << (mainClock.delta)/1000.0f << " y.p " << object1.r.y << " y.logic " << object1.pos_y << " u " << object1.v_y << std::endl;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
        SDL_RenderFillRect( renderer, &object1.r );
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}