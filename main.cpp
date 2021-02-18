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
    float resistance = 0.0f;

    RigidBody object1;
    object1.mass = 50.0f;
    int accel = 0;
    float time = 0;
    Clock mainClock;
    while (isRunning) {    

        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        while( SDL_PollEvent( &event ) ){
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        if (keyState[SDL_SCANCODE_DOWN]){
            object1.eng_y+= 50.0f*mainClock.dT;
            object1.eng_y = std::min(object1.eng_y, 100.0f);
        }
        else if (keyState[SDL_SCANCODE_UP]){
            object1.eng_y -= 50.0f*mainClock.dT;
            object1.eng_y = std::max(object1.eng_y, -100.0f);
        }
        else {
            object1.eng_y = object1.eng_y*0.05f*mainClock.dT;
            object1.eng_y = std::max(object1.eng_y, 0.0f);
        }

        // physics round
        mainClock.tick();
        //SDL_Delay(50); // to test if the dT works... large delays cause physics issues

        // natural deceleration  should be due to friction + air resistance
        // friction & air-resistance is proportional to downforce. Downforce is proportional to speed square
        // since resistance is a func of speed
        // probably should calculate resistance after calculating new speed....

        
        object1.a_y = (object1.eng_y-resistance);
        object1.v_y += object1.a_y * mainClock.dT;
        int factor = 1;
        if (object1.v_y < 0){
            factor = -1;
        }
        resistance = factor * (0.05f * object1.v_y * object1.v_y); 
        accel = 150+factor*50;

        object1.pos_y += object1.v_y * mainClock.dT;
        object1.r.y = object1.pos_y;

        if (object1.r.y > 600-20){
            object1.pos_y = 600-20;
            object1.r.y = object1.pos_y;
        }

        if (object1.r.y < 0){
            object1.pos_y = 0.0f;
            object1.r.y = object1.pos_y;
        }

        std:: cout << (int)object1.v_y << "m/s \t" << (int)object1.a_y << "accel \t" << (int)object1.eng_y << "eng \t" << (int)resistance << "wind" << std::endl;

        time = time + mainClock.dT;


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