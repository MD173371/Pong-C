#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "./constants.h"

void setup();
int processInput();
void update(int upDown);
void render();
void destroyWindow();
int initializeWindow(void);

int gameIsRunning = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int lastFrameTime = 0;

struct ball {
    float x;
    float y;
    float width;
    float height;
    int directionX;
    int directionY;
    float angle;
    float midY;
} ball;

struct paddle1 {
    float x;
    float y;
    float width;
    float height;
    float midY;
    int score;
} paddle1;

struct paddle2 {
    float x;
    float y;
    float width;
    float height;
    float midY;
    int score;
} paddle2;

struct line {
    float width;
    float height;
    float x;
    float y;
} line;

int main() {
    int upDown = 0;

    srand(time(NULL));
    
    gameIsRunning = initializeWindow();
    
    TTF_Init();
    setup();

    while (gameIsRunning) {
        upDown = processInput();
        update(upDown);
        render();
    }

    destroyWindow();

    return 0;
}

void setup() {
    ball.angle = 0.2;
    ball.x = 200;
    ball.y = 200;
    ball.width = 15;
    ball.height = 15;
    ball.directionX = 1;
    ball.directionY = 1;

    paddle1.x = 20;
    paddle1.y = 20;
    paddle1.width = 10;
    paddle1.height = 60;
    paddle1.score = 0;
    
    paddle2.x = WINDOW_WIDTH - 30;
    paddle2.y = WINDOW_HEIGHT - 80;
    paddle2.width = 10;
    paddle2.height = 60;
    paddle1.score = 0;
    
    line.width = 8;
    line.height = 15;
    line.x = WINDOW_WIDTH / 2;
}

int processInput() {
    SDL_Event event;
    SDL_PollEvent(&event);
    int upDown = 0;


    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
    if(keystates[SDL_SCANCODE_UP] && paddle1.y >= 13){
        upDown = -1;
    }
    if(keystates[SDL_SCANCODE_DOWN] && paddle1.y <= WINDOW_HEIGHT - 72){
        upDown = 1;   
    }


    switch (event.type) {
        case SDL_QUIT:
            gameIsRunning = FALSE;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                gameIsRunning = FALSE; 
            /*
            else if (event.key.keysym.sym == SDLK_UP)
            {
                if(paddle1.y >= 17)
                {
                    upDown = -1;
                }
            }
            
            else if (event.key.keysym.sym == SDLK_DOWN)
            {
                if(paddle1.y <= WINDOW_HEIGHT - 72)
                {
                    upDown = 1;
                }
            } */  
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_UP)
                upDown = 0;
            else if (event.key.keysym.sym == SDLK_DOWN)
                upDown = 0;
            break;      
    }

    return upDown;
}

void update(int upDown) {
    // waste some time / sleep until we reach the frame target time
    
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - lastFrameTime);

    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }

    // Get a delta time factor converted to seconds to be used to update my objects
    float deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0f;

    lastFrameTime = SDL_GetTicks();
   
    ball.midY = ball.y + ( ball.width / 2 );
    paddle1.midY = paddle1.y + ( paddle1.height / 2 );
    paddle2.midY = paddle2.y + ( paddle2.height / 2 );

    if (ball.x <= paddle2.x + 4 && ball.x >= paddle2.x - 15  
            && ( ball.midY <= paddle2.y + 62 && ball.midY >= paddle2.y - 12))
    {
        ball.angle = -(asinf(((ball.y + 7.5) - (paddle2.y + 30))/60)) ;
        ball.x = paddle2.x - 18;
        ball.directionX *= -1;
    }
    
    if (ball.x <= paddle1.x + 15 && ball.x >= paddle1.x - 4  && ( ball.y <= paddle1.y + 62 && ball.y >= paddle1.y - 12))
    {
        ball.angle = -(asinf(((paddle1.y + 30) - (ball.y + 7.5))/60));
        ball.x = paddle1.x + 18;
        ball.directionX *= -1;
    }
 
    if (ball.y >= WINDOW_HEIGHT - 20 || ball.y <= 0)
    {
        ball.angle = - ball.angle;
    }

    if (ball.directionX == 1 && ball.directionY == 1)
    {
        ball.x += (cosf(ball.angle) * BALL_SPEED) * deltaTime;
        ball.y += (sinf(ball.angle) * BALL_SPEED) * deltaTime;
    }
    else if (ball.directionX == -1 && ball.directionY == 1)
    {
        ball.x -= (cosf(ball.angle) * BALL_SPEED) * deltaTime;
        ball.y -= (sinf(ball.angle) * BALL_SPEED) * deltaTime;
    }
    else if (ball.directionX == 1 && ball.directionY == -1)
    {
        ball.x += (cosf(ball.angle) * BALL_SPEED) * deltaTime;
        ball.y -= (sinf(ball.angle) * BALL_SPEED) * deltaTime;
    }
    else 
    {
        ball.x -= (cosf(ball.angle) * BALL_SPEED) * deltaTime;
        ball.y += (sinf(ball.angle) * BALL_SPEED) * deltaTime;
    }

    paddle1.y += 625 * upDown * deltaTime;

    if(paddle2.y >= 13 && paddle2.y <= WINDOW_HEIGHT - 72)
    {
        paddle2.y -= 5 * (paddle2.midY - ball.midY) * deltaTime;
    }
    else if(paddle2.y < 13)
    {
        paddle2.y = 14;
    }
    else if(paddle2.y > WINDOW_HEIGHT - 72)
    {
        paddle2.y = WINDOW_HEIGHT - 73;
    }
    if ( ball.x <= 5 )
    {
        paddle1.score += 1;
        ball.y = rand() % (WINDOW_HEIGHT - 25 - 25 + 1) + 25;
        ball.x = (WINDOW_WIDTH / 2);
        ball.angle = -3.14;
        printf("%d", paddle1.score);
    }

    if ( ball.x >= (WINDOW_WIDTH - 5))
    {
        paddle2.score += 1;
        ball.y = rand() % (WINDOW_HEIGHT - 25 - 25 + 1) + 25;
        ball.x = (WINDOW_WIDTH / 2);
        ball.angle = 3.14;
        printf("%d", paddle2.score);
    }
    
}

void render() {
    
    TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 20);

    SDL_Color White = {255, 255, 255};
    
    int length1 = snprintf(NULL, 0, "%d", paddle1.score);
    char* str1 = malloc(length1 + 1);
    snprintf(str1, length1 + 1, "%d", paddle1.score);
    
    int length2 = snprintf(NULL, 0, "%d", paddle2.score);
    char* str2 = malloc(length2 + 1);
    snprintf(str2, length2 + 1, "%d", paddle2.score); 

    SDL_Surface* score1Surface =
        TTF_RenderText_Solid(Sans, str1, White);
    SDL_Texture* score1Message = SDL_CreateTextureFromSurface(renderer, score1Surface);
    
    SDL_Surface* score2Surface =
        TTF_RenderText_Solid(Sans, str2, White);
    SDL_Texture* score2Message = SDL_CreateTextureFromSurface(renderer, score2Surface);
    
    free(str1);
    free(str2);

    SDL_Rect Message1_rect;
    SDL_Rect Message2_rect;
    Message1_rect.x = WINDOW_WIDTH - 130;
    Message1_rect.y = 40;
    Message1_rect.w = 60;
    Message1_rect.h = 70;
    
    Message2_rect.x = 50;
    Message2_rect.y = 40;
    Message2_rect.w = 60;
    Message2_rect.h = 70;


    SDL_SetRenderDrawColor(renderer, 0, 10, 10, 255);
    SDL_RenderClear(renderer);

    // Draw game ball
    SDL_Rect ball_rect = { 
        (int)ball.x,
        (int)ball.y, 
        (int)ball.width, 
        (int)ball.height 
    };

    // Draw player paddle
    SDL_Rect paddle1_rect = { 
        (int)paddle1.x,
        (int)paddle1.y, 
        (int)paddle1.width, 
        (int)paddle1.height 
    };

    // Draw enemy paddle
    SDL_Rect paddle2_rect = { 
        (int)paddle2.x,
        (int)paddle2.y, 
        (int)paddle2.width, 
        (int)paddle2.height 
    };
   
    SDL_Rect line_rect = {
        (int)line.x,
        (int)line.y,
        (int)line.width,
        (int)line.height
    };

    SDL_SetRenderDrawColor(renderer, 185, 180, 10, 200);
    SDL_RenderFillRect(renderer, &ball_rect);
    SDL_RenderFillRect(renderer, &paddle1_rect);
    SDL_RenderFillRect(renderer, &paddle2_rect);
    
    for(line_rect.y = 0; line_rect.y < WINDOW_HEIGHT; line_rect.y += 25)
    {
        SDL_RenderFillRect(renderer, &line_rect);
    }
    
    SDL_RenderCopy(renderer, score1Message, NULL, &Message1_rect);
    SDL_RenderCopy(renderer, score2Message, NULL, &Message2_rect);

    SDL_FreeSurface(score1Surface);
    SDL_FreeSurface(score2Surface);
    SDL_DestroyTexture(score1Message);
    SDL_DestroyTexture(score2Message);
    TTF_CloseFont(Sans);

    SDL_RenderPresent(renderer);
}

void destroyWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int initializeWindow(void)
{
    int nonErrState;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        nonErrState = FALSE;
    }
    else {
        window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_BORDERLESS
        );
        if (!window) {
            fprintf(stderr, "Error creating SDL Window.\n");
            nonErrState = FALSE;
        }
        else {
            renderer = SDL_CreateRenderer(window, -1, 0);
            if (!renderer) {
                fprintf(stderr, "Error creating SDL Renderer.\n");
                nonErrState = FALSE;
            }
            else {
                nonErrState = TRUE;
            }
        }
    }
    return nonErrState;
}
