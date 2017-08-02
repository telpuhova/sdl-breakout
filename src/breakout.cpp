#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>

#define LEFT 80
#define RIGHT 79
#define SPACE 44
#define ESCAPE 41

typedef enum
{
    PADDLE = 0,
    BRICK
} object_type;

int SCREEN_WIDTH = 220;
int J = SCREEN_WIDTH/20 - 1;
int SCREEN_HEIGHT = 280;

class Object{
    public:
        int x, y, w, h;
        float velocity;
        SDL_Rect rect;

        void get_rect();
        void render(SDL_Renderer* renderer);
};

class Paddle: public Object{
    public:
        Paddle(){
            x = SCREEN_WIDTH/2;
            y = SCREEN_HEIGHT - 20;
            h = 10;
            w = 35;
            velocity = 0;
            get_rect();
        }
};

class Ball: public Object{
    public:
        Ball(){
            x = SCREEN_WIDTH/2;
            y = SCREEN_HEIGHT/2;
            w = 5;
            h = 5;
            velocity = 3;
            
            velocity_x = sqrt(powf(velocity, 2) / 2);
            velocity_y = velocity_x;

            max_vel = velocity_x + 0.7; // TODO: replace 0.7 with valid coefficient
            min_vel = velocity_x - 0.7;
            get_rect();
        }
        float max_vel;
        float min_vel;
        float velocity_x;
        float velocity_y;

        void move();
        bool collided(SDL_Rect& r2);
        void collision(Object& r, object_type object);
        void wall_hit();
};

class Brick: public Object{
    public:
        Brick(int x, int y){
            this->x = x;
            this->y = y;
            h = 10;
            w = 20;
            get_rect();
        }
        void destroy();
};

class Menu{
    public:
        SDL_Texture* Message;
        SDL_Rect Message_rect;
        
        Menu(SDL_Renderer* renderer);
        void clean_up();
        void render(SDL_Renderer* renderer);
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
    int time2 = start;
    int time3 = start;
    int time_diff;
    int motion_diff;
    Ball ball;
    Paddle paddle;
    Brick* bricks[4][J];
    Menu menu(my_renderer);

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < J; j++){
            bricks[i][j] = new Brick(21 * j, 11 * i);
        }
    }

    int previous_x = paddle.x;
    bool paused = false;


    //
    //MAIN CYCLE
    //
	while (!quit){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_KEYDOWN){
                if ((event.key.keysym.scancode == RIGHT) && (!paused)){ paddle.x += 5; }
                if ((event.key.keysym.scancode == LEFT) && (!paused)){ paddle.x -= 5; }
                if (event.key.keysym.scancode == ESCAPE){ quit = 1; }
                if (event.key.keysym.scancode == SPACE){ 
                    if (!paused){
                        paused = true;
                    }
                    else {
                        paused = false;
                    }
                }
                //std::cout << event.key.keysym.scancode << std::endl;
			}
			if (event.type == SDL_QUIT){
				quit = 1;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN){
				std::cout << "Mouse button down" << std::endl;
                if (!paused){
                    paused = true;
                }
                else {
                    paused = false;
                }
			}
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                SDL_GetWindowSize(my_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
                paddle.y = SCREEN_HEIGHT - 20;
            }
		}
    
        if (!paused){
            // collision calculations
            //
            // stuff that happens every iteration
            if (SDL_GetTicks() - time1 >= 0){
                time1 = SDL_GetTicks();

                paddle.get_rect();
                ball.collision(paddle, PADDLE);
                ball.wall_hit();
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < J; j++){
                        if (bricks[i][j] != NULL){
                            if (ball.collided(bricks[i][j]->rect)){
                                ball.collision(*bricks[i][j], BRICK);
                                //bricks[i]->destroy();
                                bricks[i][j] = NULL;
                            }
                        }
                    }
                }
            }
            //----checking for collisions every iteration


            // stuff that happens every 320 iterations
            time_diff = SDL_GetTicks() - time2;
            if (time_diff >= 320){
                time2 = SDL_GetTicks();
                motion_diff = paddle.x - previous_x;
                previous_x = paddle.x;
                if (motion_diff > 0){
                    paddle.velocity = 1;
                }
                else if (motion_diff < 0){
                    paddle.velocity = -1;
                }
                else{
                    paddle.velocity = 0;
                }
            }
            //----calculating paddle's velocity


            if (SDL_GetTicks() - time3 >= 10000){
                time3 = SDL_GetTicks();
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < J; j++){
                        if (bricks[i][j] != NULL){
                            bricks[i][j]->y += 10;
                        }
                    }
                }
                
            }
                
            // stuff that happens every 40 iterations. 
            // which means:
            // drawing each frame. at a rate = 25 fps
            if (SDL_GetTicks() - time >= 40){

                time = SDL_GetTicks();

                ball.move();
            
                SDL_SetRenderDrawColor(my_renderer, 0x13, 0x13, 0x13, 0xFF);
                SDL_RenderClear(my_renderer);
            
                SDL_SetRenderDrawColor(my_renderer, 0xff, 0xff, 0xff, 0xFF);
                //SDL_RenderDrawRect(my_renderer, &paddle.rect);

                paddle.render(my_renderer);
                ball.render(my_renderer);
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < J; j++){
                        if (bricks[i][j] != NULL){
                            bricks[i][j]->render(my_renderer);
                        }
                    }
                }
                //brick->render(my_renderer);

                SDL_RenderPresent(my_renderer);
            }
        }
        else{
            menu.render(my_renderer);
            SDL_RenderPresent(my_renderer);
        }
	}
	
	// CLEANING UP
	//
    menu.clean_up();
	SDL_DestroyRenderer(my_renderer);
	SDL_DestroyWindow(my_window);
	SDL_Quit();
	return 0;
}

Menu::Menu(SDL_Renderer* renderer){

    //INIT SDL_ttf 
    if( TTF_Init() == -1 ) { 
		std::cout << "SDL_ttf init error = " << TTF_GetError() << std::endl;
		return;
    }
	
    TTF_Font* Sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 300);

    if (Sans == NULL){
        std::cout << "font = null" << std::endl;
        return; 
    }

    SDL_Color White = {255, 255, 255};

    SDL_Surface* surfaceMessage;
    if (!(surfaceMessage = TTF_RenderText_Solid(Sans, "pause", White))){
        std::cout << TTF_GetError << std::endl;
    }

    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    Message_rect.w = 50;
    Message_rect.h = 20;
    Message_rect.x = (SCREEN_WIDTH - Message_rect.w)/2;
    Message_rect.y = SCREEN_HEIGHT/2 - Message_rect.h;

}

void Menu::render(SDL_Renderer* renderer){

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
}

void Menu::clean_up(){
    SDL_DestroyTexture(Message);
    TTF_Quit();
}

void Object::get_rect(){

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void Object::render(SDL_Renderer* renderer){

    //SDL_SetRenderDrawColor(ball_renderer, 0xff, 0xff, 0xff, 0xFF);
    get_rect();
    SDL_RenderDrawRect(renderer, &rect);
}

void Ball::move(){

    x += velocity_x;
    y += velocity_y;
}

void Ball::collision(Object& r, object_type object){

    if (!collided(r.rect)){ return; }
    float koef; 
    if (object == PADDLE){
        koef = r.velocity * 0.7;
        //todo
    }
    else{
        koef = 0;
    }

    //ball dimensions
    int top1 = y;
    int bottom1 = y + h;
    int left1 = x;
    int right1 = x + w;

    //paddle dimensions
    int top2 = r.y;
    int bottom2 = r.y + r.h;
    int left2 = r.x;
    int right2 = r.x + r.w;

    int new_sign_y = 1;
    if ((bottom1 - top2) <= (right1 - left2)){
        if (object == BRICK){
            velocity_y *= -1;
            move();
        }
        else{
            if (velocity_y >= 0){
                new_sign_y = -1;
            }
            float a = velocity_x + koef;
            if ((a >= min_vel) && (a <= max_vel)){
                velocity_x += koef;
                velocity_y = sqrt(fabs(powf(velocity, 2) - powf(velocity_x, 2)));
            }
            velocity_y *= new_sign_y;
            /*/
            std::cout << "---------------------------hit the PADDLE" << std::endl;
            std::cout << "velocity X = " << velocity_x << std::endl;
            std::cout << "velocity Y = " << velocity_y << std::endl;
            std::cout << std::endl;
            */

            move();
        }
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
    else if ((y <= r.y) && (velocity_y < 0)){
        velocity_y *= -1;
        /*std::cout << "---------------------------hit the CEILING" << std::endl;
        std::cout << "velocity X = " << velocity_x << std::endl;
        std::cout << "velocity Y = " << velocity_y << std::endl;
        std::cout << "velocity = " << velocity << std::endl;
        std::cout << "ball.x = " << x << std::endl;
        std::cout << "ball.y = " << y << std::endl;
        std::cout << std::endl;
        */move();
    }
    else if ((y + h) >= (r.y + r.h)){
        velocity_y *= -1;
        move();
        //game over
        //return;
    }
}

