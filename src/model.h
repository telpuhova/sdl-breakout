#ifndef MODEL_H
#define MODEL_H

typedef enum
{
    PADDLE = 0,
    BRICK
} object_type;

extern int level;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int J;
extern int I;

class Object{
    public:
        int x, y, w, h;
        float velocity;
        //SDL_Rect rect;

        //void get_rect();
        //void render(SDL_Renderer* renderer);
};

class Paddle: public Object{
    public:
        Paddle();
};

class Ball: public Object{
    public:
        Ball();
        float max_vel;
        float min_vel;
        float velocity_x;
        float velocity_y;

        void move();
        bool collided(int r2_x, int r2_y, int r2_w, int r2_h);
        void collision(Object& r, object_type object);
        int wall_hit();
};

class Brick: public Object{
    public:
        Brick(int x, int y);
        void destroy();
};

class Game{
    public:
        Game();

        Ball ball;
        Paddle paddle;
        Brick*** bricks;
};

#endif
