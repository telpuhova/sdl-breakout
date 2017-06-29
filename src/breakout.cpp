#include <iostream>
#include <SDL.h>

#define LEFT 80
#define RIGHT 79
#define SPACE 44
#define ESCAPE 41

int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;

class Ball{
    public:
        Ball();
        static const int w = 5;
        static const int h = 5;
        static const int max_vel = 10;
        int x, y;
        int velocity_x;
        int velocity_y;
        SDL_Rect ball_rect;

        void get_rect();
        void move();
        void render(SDL_Renderer* ball_renderer);
        bool collided(SDL_Rect& r2);
        void collision(SDL_Rect& r);
        void wall_hit();
};

class Paddle{
    public:
        Paddle();
        static const int w = 30;
        static const int h = 10;
        int y;
        int x;
        SDL_Rect paddle_rect;

        void get_rect();
        void handleEvent(SDL_Event& event);
        void render(SDL_Renderer* paddle_renderer);
};


int main(){
	// INIT
	//
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ){
		std::cout << "sdl init error = " << SDL_GetError() << std::endl;
		return 1;
	}

	// WINDOW
	//
	SDL_Window *my_window = SDL_CreateWindow("breakout game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (my_window == NULL){
		std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	// RENDERER
	//
	SDL_Renderer *my_renderer = SDL_CreateRenderer(my_window, -1, SDL_RENDERER_ACCELERATED);
	if (my_renderer == NULL){
		SDL_DestroyWindow(my_window);
		std::cout << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
    }

	// variables initialasation
	//
	SDL_Event event;
	int quit = 0;
    int start = SDL_GetTicks();
    int time = start;
    int time1 = start;
    Ball ball;
    Paddle paddle;

    //
    //MAIN CYCLE
    //
	while (!quit){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.scancode == RIGHT){ paddle.x += 5; }
                if (event.key.keysym.scancode == LEFT){ paddle.x -= 5; }
                if (event.key.keysym.scancode == ESCAPE){ quit = 1; }

			}
			if (event.type == SDL_QUIT){
				quit = 1;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN){
				std::cout << "Mouse button down" << std::endl;
			}
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                SDL_GetWindowSize(my_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
                paddle.y = SCREEN_HEIGHT - 20;
            }
		}
    
        // collision calculations
        if (SDL_GetTicks() - time1 >= 0){
            time1 = SDL_GetTicks();
            paddle.get_rect();
            ball.collision(paddle.paddle_rect);
            ball.wall_hit();
        }

        // drawing of a frame. 25 fps
        if (SDL_GetTicks() - time >= 40){

            time = SDL_GetTicks();

            ball.move();
        
            SDL_SetRenderDrawColor(my_renderer, 0x13, 0x13, 0x13, 0xFF);
            SDL_RenderClear(my_renderer);
        
            SDL_SetRenderDrawColor(my_renderer, 0xff, 0xff, 0xff, 0xFF);
            SDL_RenderDrawRect(my_renderer, &paddle.paddle_rect);

            //SDL_RenderDrawRect(my_renderer, &walls);

            ball.render(my_renderer);

            SDL_RenderPresent(my_renderer);
		}    
	}
	
	// CLEANING UP
	//
	SDL_DestroyRenderer(my_renderer);
	SDL_DestroyWindow(my_window);
	SDL_Quit();
	return 0;
}


Ball::Ball(){
    x = SCREEN_WIDTH/2;
    y = SCREEN_HEIGHT/2;
    
    velocity_x = 2;
    velocity_y = 2;

    get_rect();
}

Paddle::Paddle(){
    x = SCREEN_WIDTH/2;
    y = SCREEN_HEIGHT - 20;

    get_rect();
}

void Ball::get_rect(){

    ball_rect.x = x;
    ball_rect.y = y;
    ball_rect.w = w;
    ball_rect.h = h;
}

void Paddle::get_rect(){

    paddle_rect.x = x;
    paddle_rect.y = y;
    paddle_rect.w = w;
    paddle_rect.h = h;
}

void Ball::move(){

    x += velocity_x;
    y += velocity_y;
}

void Ball::render(SDL_Renderer* ball_renderer){

    //SDL_SetRenderDrawColor(ball_renderer, 0xff, 0xff, 0xff, 0xFF);
    get_rect();
    SDL_RenderDrawRect(ball_renderer, &ball_rect);
}

void Ball::collision(SDL_Rect& r){
    if (!collided(r)){ return; }

    int top1 = y;
    int bottom1 = y + h;
    int left1 = x;
    int right1 = x + w;

    int top2 = r.y;
    int bottom2 = r.y + r.h;
    int left2 = r.x;
    int right2 = r.x + r.w;

    if ((bottom1 - top2) <= (right1 - left2)){
        velocity_y *= -1;
        move();
    }
    if ((right1 - left2) < (bottom1 - top2)){
        velocity_x *= -1;
        move();
    }
}

bool Ball::collided(SDL_Rect& r2){
    //SDL_Rect& r1 = ball_rect; 
    if ((x + w) < r2.x){ return false; } // r1 is on the left of r2
    if ((r2.x + r2.w) < x){ return false; } // r2 is on the left of r1
    if ((y + h) < r2.y){ return false; } // r1 is above r2
    if ((r2.y + r2.h) < y){ return false; } // r1 is below r2
    return true;
}

void Ball::wall_hit(){
    SDL_Rect r;
    r.x = 1;
    r.y = 1;
    r.w = SCREEN_WIDTH - 2;
    r.h = SCREEN_HEIGHT - 2;
    if (((x + w) >= (r.x + r.w)) || (x <= r.x)){
        velocity_x *= -1;
        move();
    }
    if (y <= r.y){
        velocity_y *= -1;
        move();
    }
    if ((y + h) >= (r.y + r.h)){
        //game over
        return;
    }
}

