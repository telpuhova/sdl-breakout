#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include "model.h"

#define RIGHT 79
#define LEFT 80
#define DOWN 81
#define UP 82
#define ENTER 40
#define SPACE 44
#define ESCAPE 41


class Menu{
    public:
        const char* start_message[3];
        const char* pause_message[3]; 
        const char* lost_message[3]; 
        const char* won_message[3];
        SDL_Texture* texture_message[3];
        SDL_Rect message_rect[3];

        SDL_Rect the_dot;
        int state; // 0: start; 1: pause; 2: lost; 3: won;
        int choice; // 0/1: menu options

        TTF_Font* font;
        SDL_Color color;
        
        Menu(SDL_Renderer* renderer);
        void start(SDL_Renderer* renderer);
        void pause(SDL_Renderer* renderer);
        void lost(SDL_Renderer* renderer);
        void won(SDL_Renderer* renderer);
        void clean_up();
        void render(SDL_Renderer* renderer);
        void dot_change();
        SDL_Texture* make_a_mess(const char* data, SDL_Renderer* renderer);
};

class Render{
    public:
        SDL_Renderer* my_renderer;
        SDL_Window* my_window;

        //Render();
        int init();
        void render(Object& o);
        void render_brick(Brick* o);
        ~Render(){
            // CLEANING UP
            //
            //
            SDL_DestroyRenderer(my_renderer);
            SDL_DestroyWindow(my_window);
            SDL_Quit();
        }
};

int loop(Render& render);

int main(){

    Render render;
    render.init();

    //level = 0;
    
    while (loop(render));
}


int loop(Render& render){
    // variables initialasation
	//
    Game game;
	SDL_Event event;
	int quit = 0;
    int start = SDL_GetTicks();
    int time = start;
    int time1 = start;
    int time2 = start;
    int time3 = start;
    int time_diff;
    int motion_diff;
    Menu menu(render.my_renderer);

    int previous_x = game.paddle.x;
    bool thereissomething;
    bool paused = false;

    // printing level number in the right bottom corner
    char* ptr = "level ";
    char* data;
    data = new char[10];
    char* data_ptr = data;
    while (*ptr != '\0'){
        *data_ptr = *ptr;
        ptr++;
        data_ptr++;
    }
    *data_ptr = (char)(48 + level);
    data_ptr++;
    *data_ptr = '\0';

    SDL_Rect level_rect;
    level_rect.x = SCREEN_WIDTH - 50;
    level_rect.y = SCREEN_HEIGHT - 20;
    level_rect.w = 45;
    level_rect.h = 15;

    for (int i = 0; i < I; i++){
        for (int j = 0; j < J; j++){
            if (game.bricks[i][j] != NULL){
                game.bricks[i][j]->y += game.bricks[i][j]->h;
                game.bricks[i][j]->x += game.bricks[i][j]->w/4;
            }
        }
    }

    menu.start(render.my_renderer);
    //
    //MAIN CYCLE
    //
	while (!quit){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_KEYDOWN){
                if (!paused){
                    if (event.key.keysym.scancode == RIGHT){ game.paddle.x += 5; }
                    if (event.key.keysym.scancode == LEFT){ game.paddle.x -= 5; }
                    if (event.key.keysym.scancode == SPACE){ 
                        menu.pause(render.my_renderer);
                        menu.state = 1;
                        paused = true;
                    }
                }
                if (paused){
                    if ((event.key.keysym.scancode == UP) || (event.key.keysym.scancode == DOWN)){
                        menu.dot_change();
                        menu.choice = menu.choice ^ 1;
                    }
                    if (event.key.keysym.scancode == ENTER){ 
                        if (menu.choice == 0){
                            switch (menu.state){
                                case 0:
                                    paused = false;
                                    break;
                                case 1:
                                    paused = false;
                                    break;
                                case 2:
                                    menu.clean_up();
                                    return 1;
                                case 3:
                                    level++;
                                    I = level + 1;
                                    menu.clean_up();
                                    return 1;
                                }
                            }
                            else{
                            quit = true;
                        }
                    }
                }
                if (event.key.keysym.scancode == ESCAPE){ quit = 1; }
			}

			if (event.type == SDL_QUIT){
				quit = 1;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN){
                if (!paused){
                    menu.pause(render.my_renderer);
                    paused = true;
                }
                else {
                    paused = false;
                }
			}
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){
                SDL_GetWindowSize(render.my_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
                game.paddle.y = SCREEN_HEIGHT - 20;
            }
		}
    
        if (!paused){
            // collision calculations
            //
            // stuff that happens every iteration
            if (SDL_GetTicks() - time1 >= 0){
                time1 = SDL_GetTicks();

                game.ball.collision(game.paddle, PADDLE);
                if (game.ball.wall_hit()){
                    menu.lost(render.my_renderer);
                    menu.state = 2;
                    paused = true;
                }

                thereissomething = false;
                for (int i = 0; i < I; i++){
                    for (int j = 0; j < J; j++){
                        if (game.bricks[i][j] != NULL){
                            thereissomething = true;
                            if (game.ball.collided(game.bricks[i][j]->x, game.bricks[i][j]->y, game.bricks[i][j]->w, game.bricks[i][j]->h)){
                                game.ball.collision(*game.bricks[i][j], BRICK);
                                delete game.bricks[i][j];
                                game.bricks[i][j] = NULL;
                            }
                        }
                    }
                }
                if (!thereissomething){
                    menu.won(render.my_renderer);
                    menu.state = 3;
                    paused = true;
                }
            }
            //----checking for collisions every iteration


            // stuff that happens every 320 iterations
            time_diff = SDL_GetTicks() - time2;
            if (time_diff >= 320){
                time2 = SDL_GetTicks();
                motion_diff = game.paddle.x - previous_x;
                previous_x = game.paddle.x;
                if (motion_diff > 0){
                    game.paddle.velocity = 1;
                }
                else if (motion_diff < 0){
                    game.paddle.velocity = -1;
                }
                else{
                    game.paddle.velocity = 0;
                }
            }
            //----calculating paddle's velocity


            if (SDL_GetTicks() - time3 >= 100000){
                time3 = SDL_GetTicks();
                for (int i = 0; i < I; i++){
                    for (int j = 0; j < J; j++){
                        if (game.bricks[i][j] != NULL){
                            game.bricks[i][j]->y += 10;
                        }
                    }
                }
                
            }
                
            // stuff that happens every 40 iterations. 
            // which means:
            // drawing each frame. at a rate = 25 fps
            if (SDL_GetTicks() - time >= 40){

                time = SDL_GetTicks();

                game.ball.move();
            
                SDL_SetRenderDrawColor(render.my_renderer, 0x13, 0x13, 0x13, 0xFF);
                SDL_RenderClear(render.my_renderer);
            
                SDL_SetRenderDrawColor(render.my_renderer, 0xff, 0xff, 0xff, 0xFF);
                //SDL_RenderDrawRect(render.my_renderer, &paddle.rect);




                //SDL_Surface* surface_message = TTF_RenderText_Solid(menu.font, data, menu.color);
                //SDL_Texture* texture_message = SDL_CreateTextureFromSurface(render.my_renderer, surface_message);
                //SDL_RenderCopy(render.my_renderer, texture_message, NULL, &level_rect);

                render.render(game.paddle);
                render.render(game.ball);
                for (int i = 0; i < I; i++){
                    for (int j = 0; j < J; j++){
                        if (game.bricks[i][j] != NULL){
                            render.render_brick(game.bricks[i][j]);
                        }
                    }
                }

                SDL_RenderPresent(render.my_renderer);
            }
        }
        else{
            if (SDL_GetTicks() - time >= 40){

                time = SDL_GetTicks();

                SDL_SetRenderDrawColor(render.my_renderer, 0x13, 0x13, 0x13, 0xFF);
                SDL_RenderClear(render.my_renderer);
                SDL_SetRenderDrawColor(render.my_renderer, 0xff, 0xff, 0xff, 0xFF);

                menu.render(render.my_renderer);
                SDL_RenderPresent(render.my_renderer);
            }
        }
	}
	
    menu.clean_up();
    delete[] data;

    for (int i = 0; i < I; i++){
        for (int j = 0; j < J; j++){
            delete game.bricks[i][j];
        }
    }
	return 0;
}

int Render::init(){
	// INIT
	//
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ){
		std::cout << "sdl init error = " << SDL_GetError() << std::endl;
		return 1;
	}

	// WINDOW
	//
	my_window = SDL_CreateWindow("breakout game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (my_window == NULL){
		std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// RENDERER
	//
	my_renderer = SDL_CreateRenderer(my_window, -1, SDL_RENDERER_ACCELERATED);
	if (my_renderer == NULL){
		SDL_DestroyWindow(my_window);
		std::cout << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
    }

    SDL_RaiseWindow(my_window);
    return 0;
}

void Render::render(Object& o){

    SDL_Rect rect;

    rect.x = o.x;
    rect.y = o.y;
    rect.w = o.w;
    rect.h = o.h;

    SDL_RenderDrawRect(my_renderer, &rect);
}

void Render::render_brick(Brick* o){

    SDL_Rect rect;

    rect.x = o->x;
    rect.y = o->y;
    rect.w = o->w;
    rect.h = o->h;

    SDL_RenderDrawRect(my_renderer, &rect);
}

Menu::Menu(SDL_Renderer* renderer){

    state = 0;
    choice = 0;

    //int zero_point = SCREEN_HEIGHT/3 - message_rect[0].h;
    int zero_point = SCREEN_HEIGHT/3 - 15;

    for (int i = 0; i < 3; i++){
        message_rect[i].w = 90;
        message_rect[i].h = 15;
        message_rect[i].x = (SCREEN_WIDTH - message_rect[i].w)/2;
        message_rect[i].y = zero_point;
        zero_point += message_rect[i].h + 10 * (2 - i);
    }

    the_dot.w = 4;
    the_dot.h = 4;
    the_dot.x = message_rect[1].x - 10;
    the_dot.y = message_rect[1].y + message_rect[1].h/2 - the_dot.h/2;

    //INIT SDL_ttf 
    if( TTF_Init() == -1 ) { 
		std::cout << "SDL_ttf init error = " << TTF_GetError() << std::endl;
		return;
    }
	
    font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 300);

    if (font == NULL){
        std::cout << "font = null" << std::endl;
        return; 
    }

    color = {255, 255, 255};

    start_message[0] = " breakout! ";
    start_message[1] = "   play    ";
    start_message[2] = "   quit      ";

    pause_message[0] = "   pause   ";
    pause_message[1] = "   resume  ";
    pause_message[2] = "   quit      ";

    lost_message[0] = "    :(      ";
    lost_message[1] = " start again";
    lost_message[2] = "    quit      ";

    won_message[0] = "     :D      ";
    won_message[1] = "  next level ";
    won_message[2] = "     quit      ";

}


void Menu::dot_change(){
    if (!choice){
        the_dot.x = message_rect[2].x - 10;
        the_dot.y = message_rect[2].y + message_rect[2].h/2 - the_dot.h/2;
    }
    else{
        the_dot.x = message_rect[1].x - 10;
        the_dot.y = message_rect[1].y + message_rect[1].h/2 - the_dot.h/2;
    }
}

void Menu::start(SDL_Renderer* renderer){

    for (int i = 0; i<3; i++){
        texture_message[i] = make_a_mess(start_message[i], renderer);
    }
}

void Menu::pause(SDL_Renderer* renderer){

    for (int i = 0; i<3; i++){
        texture_message[i] = make_a_mess(pause_message[i], renderer);
    }
}

void Menu::lost(SDL_Renderer* renderer){

    for (int i = 0; i<3; i++){
        texture_message[i] = make_a_mess(lost_message[i], renderer);
    }
}

void Menu::won(SDL_Renderer* renderer){

    for (int i = 0; i<3; i++){
        texture_message[i] = make_a_mess(won_message[i], renderer);
    }
}

SDL_Texture* Menu::make_a_mess(const char* data, SDL_Renderer* renderer){

    SDL_Surface* surface_message = TTF_RenderText_Solid(font, data, color);
    SDL_Texture* texture_message = SDL_CreateTextureFromSurface(renderer, surface_message);
    return texture_message;
}

void Menu::render(SDL_Renderer* renderer){

    //std::cout << "menu::render" << std::endl;
    for (int i = 0; i < 3; i++){
        SDL_RenderCopy(renderer, texture_message[i], NULL, &message_rect[i]);
    }

    SDL_RenderDrawRect(renderer, &the_dot);
}

void Menu::clean_up(){
    for (int i = 0; i < 3; i++){
        SDL_DestroyTexture(texture_message[i]);
    }
    TTF_Quit();
}

