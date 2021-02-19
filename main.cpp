// WARNING ULTRA DIRTY CODE - just my experimental playground
// playing around to revive my memory into basic linear physics implementation in code
// simple cube that accelerates by a "engine accel force"
// and that decelerates by air resistance
// while figuring out c++ in parallel

// some mass-spring-damper
// some dual-link-mass-spring-damper

#include <iostream>
#include <algorithm>
#include <math.h>
#include <SDL2/SDL.h>

struct RigidBody {
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

struct Box {
    SDL_Rect pos = {
        .x = 0,
        .y = 0,
        .w = 5,
        .h = 5,
    };
};

struct SpringLink
{
    float pos1X=0;
    float pos1Y=0;
    float pos2X=0;
    float pos2Y=0;
    float k=0;
    float damping=0;
    float mass=0;
    float forceX=0;
    float forceY=0;
    float velX=0;
    float velY=0;
};


struct Clock
{
    uint32_t last_tick_time = 0;
    uint32_t tick_time = 0;
    uint32_t delta = 0;
    float dT = 0.000000001f;

    void tick()
    {
        tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
        dT = (float)delta/1000.0f;
        last_tick_time = tick_time;
    }
};

int main(int argv, char** args)
{
    SDL_Init(SDL_INIT_EVERYTHING);


    SDL_Window *window = SDL_CreateWindow("Air resistance", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool isRunning = true;
    SDL_Event event;
    float resistance = 0.0f;

    RigidBody object1;
    object1.mass = 50.0f;
    RigidBody object2;

    RigidBody anchor;
    
    int accel = 0;
    float time = 0;
    Clock mainClock;
    const float accel_rate = 100.0f; // rate of acceleration per second
    const float max_power = 300.0f; // saturating power
    const float drag = 0.02f; // a fake drag factor

    
    // springmass physics
    float gravity = 10;
    float mass = 10;
    float posY = 50;
    float posX = 220;
    const float dampingC = 6;
    float stiff = 20;
    
    object2.r.y = posY;
    object2.r.x = posX;
    
    float velocityX = 0;
    float velocityY = 0;
    float timeStep = 0.02;
    float anchorX = 200;
    float anchorY = 300; 
    anchor.r.x = anchorX;
    anchor.r.y = anchorY;
    
    SpringLink link1;
    SpringLink link2;

    link1.mass = 10;
    link1.k = 20;
    link1.damping = 6;
    link1.pos1X = 450;
    link1.pos1Y = 150;
    link1.pos2X = 300;
    link1.pos2Y = 100;

    link2.mass = 10;
    link2.k = 5;
    link2.damping = 10;
    link2.pos1X = link1.pos2X;
    link2.pos1Y = link1.pos2Y;
    link2.pos2X = 200;
    link2.pos2Y = 50;

    while (isRunning) {    
        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        while( SDL_PollEvent( &event ) ){
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        if (keyState[SDL_SCANCODE_DOWN]){
            object1.eng_y+= accel_rate *mainClock.dT;
            object1.eng_y = std::min(object1.eng_y, max_power);
        }
        else if (keyState[SDL_SCANCODE_UP]){
            object1.eng_y -= accel_rate *mainClock.dT;
            object1.eng_y = std::max(object1.eng_y, -max_power);
        }
        else{
            object1.eng_y = object1.eng_y * (1 - 0.9f)* mainClock.dT;
        }
        mainClock.tick();
        // physics round
        
        //SDL_Delay(50); // to test if the dT works... large delays cause physics issues

        // natural deceleration  should be due to friction + air resistance
        // friction & air-resistance is proportional to downforce. Downforce is proportional to speed square
        // since resistance is a func of speed
        // probably should calculate resistance after calculating new speed....

        if (object1.r.y > 600-20){
            object1.pos_y = 600-20;
            object1.r.y = object1.pos_y;
            object1.a_y = 0;
            object1.v_y = 0;
        }

        if (object1.r.y < 0){
            object1.pos_y = 0.0f;
            object1.r.y = object1.pos_y;
            object1.a_y = 0;
            object1.v_y = 0;
        }
        
        object1.a_y = (object1.eng_y-resistance);
        object1.v_y += object1.a_y * mainClock.dT;
        int factor = 1;
        if (object1.v_y < 0){
            factor = -1;
        }
        resistance = factor * (drag * object1.v_y * object1.v_y); 
        accel = 150+factor*50;

        object1.pos_y += object1.v_y * mainClock.dT;
        object1.r.y = object1.pos_y;


        std:: cout << (int)object1.v_y << " m/s \t" << (int)object1.a_y << " accel \t" << (int)object1.eng_y << " eng \t" << (int)resistance << " wind" << std::endl;
        
        // basic trial
        float springFY = -stiff * (object2.r.y - anchorY);
        float springFX = -stiff * (object2.r.x - anchorX);
        float dampingFY = dampingC * velocityY;
        float dampingFX = dampingC * velocityX;
        float forceY = springFY + mass * gravity - dampingFY;
        float forceX = springFX - dampingFX;
        float accelY = forceY/mass; // duh = accelY = gravity
        float accelX = forceX/mass; // duh = accelY = gravity       
        velocityY += accelY * mainClock.dT;
        velocityX += accelX * mainClock.dT;      
        posX += velocityX * mainClock.dT;
        posY += velocityY * mainClock.dT;
        object2.r.y = posY;
        object2.r.x = posX;

        // multi-link trial
        link2.pos1Y = link1.pos2Y;
        link2.pos1X = link1.pos2X;


        float spring1FY = -link1.k * (link1.pos2Y - link1.pos1Y);
        float spring1FX = -link1.k * (link1.pos2X - link1.pos1X);
    
        float spring2FY = -link1.k * (link2.pos2Y - link2.pos1Y);
        float spring2FX = -link1.k * (link2.pos2X - link2.pos1X);

        float damp1FY = link1.damping * link1.velY;
        float damp1FX = link1.damping * link1.velX;

        float damp2FY = link2.damping * link2.velY;
        float damp2FX = link2.damping * link2.velX;

        link1.forceY = spring1FY + mass * gravity - damp1FY;
        link1.forceX = spring1FX  - damp1FX;

        link2.forceY = spring2FY + mass * gravity - damp2FY;
        link2.forceX = spring2FX  - damp2FX;

        link1.velY += link1.forceY/link1.mass * mainClock.dT;
        link1.velX += link1.forceX/link1.mass * mainClock.dT;

        link2.velY += link2.forceY/link2.mass * mainClock.dT;
        link2.velX += link2.forceX/link2.mass * mainClock.dT;

        link1.pos2Y += link1.velY * mainClock.dT;
        link1.pos2X += link1.velX * mainClock.dT;

        //link2.pos1Y = link1.pos1Y;
        //link2.pos1X = link1.pos1X;

        link2.pos2Y += link2.velY * mainClock.dT;
        link2.pos2X += link2.velX * mainClock.dT;         
        
        Box box1;
        Box box2;
        Box box3;

        box1.pos.y = link1.pos1Y;
        box1.pos.x = link1.pos1X;

        box2.pos.y = link1.pos2Y;
        box2.pos.x = link1.pos2X;

        box3.pos.y = link2.pos2Y;
        box3.pos.x = link2.pos2X;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor( renderer, 0, 0, accel, 255 );
        SDL_RenderFillRect( renderer, &object1.r );


/*
        SDL_SetRenderDrawColor( renderer, 100, 100, 150, 255 );
        SDL_RenderDrawLine(renderer, link1.pos1X, link1.pos1Y, link1.pos2X, link1.pos2Y);
        SDL_RenderDrawLine(renderer, link2.pos1X, link2.pos1Y, link2.pos2X, link2.pos2Y);
        */


        SDL_RenderFillRect( renderer, &object2.r );
        SDL_RenderFillRect( renderer, &anchor.r );
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLineF(renderer, object2.r.x+10, object2.r.y+10, anchor.r.x+10, anchor.r.y+10);
        SDL_RenderDrawLineF(renderer, link1.pos1X, link1.pos1Y, link1.pos2X, link1.pos2Y);
        SDL_RenderDrawLineF(renderer, link2.pos1X, link2.pos1Y, link2.pos2X, link2.pos2Y);
        SDL_RenderDrawPointF(renderer, link1.pos1X, link1.pos1Y);
        SDL_RenderDrawPointF(renderer, link2.pos1X, link1.pos2Y);
        SDL_RenderDrawPointF(renderer, link2.pos2X, link2.pos2Y);
        SDL_RenderFillRect(renderer, &box1.pos);
        SDL_RenderFillRect(renderer, &box2.pos);
        SDL_RenderFillRect(renderer, &box3.pos);
        SDL_RenderFillRect(renderer, &object2.r );
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
