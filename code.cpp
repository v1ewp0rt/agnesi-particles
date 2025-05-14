#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <array>

using namespace std; 

struct References { float p, n, s, w, e; };
struct Particle { float x, y; int k; };

SDL_Window *window;
SDL_Renderer* renderer;
SDL_Event event;
vector<Particle> particles;
float range = 0.02;
float mass = 20; 
float scale = 3; 
float energyThreshold = 0;
int renderScale = 6;
int screenWidth = 1920/renderScale;
int screenHeight = 1080/renderScale;
bool paused = true;
bool dType = 0;

void main_window(int w=screenWidth, int h=screenHeight) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Main", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w*renderScale, h*renderScale, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer, renderScale, renderScale);
}
void pixel(int x, int y, int r=255, int g=255, int b=200) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
float get_energy_value(float x, float y) {
    float value = 0;
    for (Particle &p : particles) {
        float dx = x-p.x; 
        float dy = y-p.y;
        value += p.k*mass/(1+dx*dx+dy*dy);
    } return value;
}
References get_energy_values(float x, float y) {
    References values = {0, 0, 0, 0, 0};
    for (Particle &p : particles) {
        float dx = x-p.x; 
        float dy = y-p.y;
        values.p += p.k*mass/(1+dx*dx+dy*dy);
        values.n += p.k*mass/(1+dx*dx+(dy+range)*(dy+range));
        values.s += p.k*mass/(1+dx*dx+(dy-range)*(dy-range));
        values.w += p.k*mass/(1+(dx-range)*(dx-range)+dy*dy);
        values.e += p.k*mass/(1+(dx+range)*(dx+range)+dy*dy);
    } return values;
}
void generate_particles(int amount=1000) {
    for (int p=0; p<amount/3; p++) {
        Particle newParticle;
        float angle = float(rand()%7000)/1000;
        float magnitude = float(p)/100;
        newParticle.x = cos(angle)*magnitude;
        newParticle.y = sin(angle)*magnitude;
        newParticle.k = rand()%100<=50?1:-1;
        particles.push_back(newParticle);
    } for (int p=0; p<amount/3; p++) {
        Particle newParticle;
        float angle = float(rand()%7000)/1000;
        float magnitude = float(p)/100;
        newParticle.x = -25+cos(angle)*magnitude;
        newParticle.y = sin(angle)*magnitude;
        newParticle.k = rand()%100<=51?1:-1;
        particles.push_back(newParticle);
    } for (int p=0; p<amount/3; p++) {
        Particle newParticle;
        float angle = float(rand()%7000)/1000;
        float magnitude = float(p)/100;
        newParticle.x = 25+cos(angle)*magnitude;
        newParticle.y = sin(angle)*magnitude;
        newParticle.k = rand()%100<=49?1:-1;
        particles.push_back(newParticle);
    }
}
void draw_particle(Particle &p, References &values) {
    if (p.x<(screenWidth/2)/scale && p.y>(-screenHeight/2)/scale) {
        float red = 100+(p.k==1)*80-(values.p<energyThreshold)*100;
        float green = (values.p<energyThreshold)*180;
        float blue = 100+(p.k==-1)*80-(values.p>energyThreshold)*100;
        pixel(p.x*scale+screenWidth/2, p.y*scale+screenHeight/2, red, green, blue); 
    } 
}
void draw_field(int size=100, float resolution=0.75) {
    for (float y=-size/2; y<size/2; y+=1/resolution) {
        for(float x=-size/2; x<size/2; x+=1/resolution) {
            float energy = get_energy_value(x, y);
            if (x<(screenWidth/2)/scale && y>(-screenHeight/2)/scale) {
                float red = (energy>energyThreshold?energy:0)*50-(energy*20);
                float green = (energy<energyThreshold?-energy:0)*50;
                float blue = (energy<energyThreshold?-energy:0)*50+(energy*20);
                red = red<0?0:red>255?255:red;
                blue = blue<0?0:blue>255?255:blue;
                green = green<0?0:green>255?255:green;
                pixel(x*scale+screenWidth/2, y*scale+screenHeight/2, red, green, blue); 
            } 
        }
    }
}
void update(bool drawType=1) {
    SDL_RenderClear(renderer);
    for (Particle &p : particles) {
        References values=get_energy_values(p.x, p.y);
        array<float, 2> forces = {values.s-values.n, values.w-values.e}; 
        if (drawType==0) { draw_particle(p, values); }
        float xForceValue = p.k*forces[1];
        float yForceValue = p.k*forces[0];
        if (!paused) { p.x += xForceValue; p.y += yForceValue; }
    } if (drawType==1) { draw_field(); }
    SDL_RenderPresent(renderer);
}
int main() {
    generate_particles();
    main_window();
    while(true) { 
        update(dType);
        const Uint8* key = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { SDL_Quit(); exit(0); }
            if (event.type == SDL_KEYDOWN) {
                if (key[SDL_SCANCODE_SPACE]) { paused = !paused;  }
                if (key[SDL_SCANCODE_W]) { dType = !dType; }
            }
        } if (key[SDL_SCANCODE_Q]) { scale -= 0.05*scale; } 
        if (key[SDL_SCANCODE_E]) { scale += 0.05*scale; }
        
    }
}