#include <iostream>
#include <math.h>
#include "model.h"


int level = 4;
int SCREEN_WIDTH = 220;
int SCREEN_HEIGHT = 280;
int J = SCREEN_WIDTH/20 - 1;
int I = level + 1;

Game::Game(){

    bricks = new Brick**[I];
    for (int i = 0; i<I; i++){
        bricks[i] = new Brick*[J];
    }
    for (int i = 0; i < I; i++){
        for (int j = 0; j < J; j++){
            bricks[i][j] = new Brick(21 * j, 11 * i);
        }
    }
}

Paddle::Paddle(){
    x = SCREEN_WIDTH/2;
    y = SCREEN_HEIGHT - 20;
    h = 10;
    w = 35;
    velocity = 0;
    //get_rect();
}

Ball::Ball(){
    x = SCREEN_WIDTH/2;
    y = SCREEN_HEIGHT/2;
    w = 5;
    h = 5;
    velocity = 3 + level/4;
    
    velocity_x = sqrt(powf(velocity, 2) / 2);
    velocity_y = velocity_x;

    max_vel = velocity_x + 0.7; // TODO: replace 0.7 with valid coefficient
    min_vel = velocity_x - 0.7;
    //get_rect();
}

Brick::Brick(int x, int y){
    this->x = x;
    this->y = y;
    h = 10;
    w = 20;
    //get_rect();
}

void Ball::move(){

    x += velocity_x;
    y += velocity_y;
}

void Ball::collision(Object& r, object_type object){

    if (!collided(r.x, r.y, r.w, r.h)){ return; }
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

bool Ball::collided(int r2_x, int r2_y, int r2_w, int r2_h){
    //SDL_Rect& r1 = ball_rect; 
    if ((x + w) < r2_x){ return false; } // r1 is on the left of r2
    if ((r2_x + r2_w) < x){ return false; } // r2 is on the left of r1
    if ((y + h) < r2_y){ return false; } // r1 is above r2
    if ((r2_y + r2_h) < y){ return false; } // r1 is below r2
    return true;
}

int Ball::wall_hit(){
    //SDL_Rect r;
    int r_x = 1;
    int r_y = 1;
    int r_w = SCREEN_WIDTH - 2;
    int r_h = SCREEN_HEIGHT - 2;
    if (((x + w) >= (r_x + r_w)) || (x <= r_x)){
        velocity_x *= -1;
        move();
    }
    else if ((y <= r_y) && (velocity_y < 0)){
        velocity_y *= -1;
        move();
    }
    else if ((y + h) >= (r_y + r_h)){
        velocity_y *= -1;
        move();
        //game over
        return 1;
    }
    return 0;
}

