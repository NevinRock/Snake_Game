#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

// global define
#define OBJ_WIDTH 20
#define OBJ_HEIGHT 20

int WINDOW_WIDTH;
int WINDOW_HEIGHT;

SDL_Window *win = nullptr;
SDL_Renderer *rdr = nullptr;
SDL_Surface *bg = nullptr;
SDL_Texture *bg_texture = nullptr;

// strct for nake
struct snake_body
{
    SDL_Rect body;
    int dx, dy;
};

std::vector<snake_body> snakes;

// stuct for food
struct food_location
{
    SDL_Rect body;
};
std::vector<food_location> foods;

// stuct for band
struct band_location
{
    SDL_Rect body;
};
std::vector<band_location> bands;

// overlap checking
template <typename T>
SDL_bool overlap_check(const std::vector<snake_body> &snake , const std::vector<T> &obj)
{
    SDL_bool output = SDL_FALSE;
    for (const snake_body &x : snake)
    {
        for (const T &y : obj)
        {
            output = SDL_HasIntersection(&x.body, &y.body);
        }
    }

    return output;
}

bool boundary_check(const std::vector<snake_body> &snake)
{
    if (snake[0].body.x >= WINDOW_WIDTH || snake[0].body.y >= WINDOW_HEIGHT || snake[0].body.x <= 0 || snake[0].body.y <= 0)
    {
        return true;
    }

    return false;
}

int Init()
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) //success: 0, error: negative
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
    {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return 1;
    }


    win = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_RESIZABLE);
    // check win
    if (win == NULL)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    rdr = SDL_CreateRenderer(win, -1, 0);
    if (rdr == NULL)
    {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetRenderDrawBlendMode(rdr, SDL_BLENDMODE_BLEND);

    bg = IMG_Load("../media/001.jpg");
    WINDOW_WIDTH = bg->w;
    WINDOW_HEIGHT = bg->h;

    if (bg== nullptr)
    {
        std::cout << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return 1;
    }
    SDL_SetWindowSize(win, bg->w, bg->h);

    bg_texture = SDL_CreateTextureFromSurface(rdr, bg);
    if (bg_texture == nullptr)
    {
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return 1;
    }



    for (int i = 0; i < 5; i++)
    {
        snakes.push_back({{200-25*i, 200, OBJ_WIDTH,OBJ_HEIGHT}, OBJ_WIDTH+5, 0});
    }

    return 0;
}

int Draw()
{
    SDL_RenderClear(rdr);
    SDL_RenderCopy(rdr, bg_texture, NULL, NULL);

    //snake movement
    for (int i = snakes.size(); i > 0; i--)
    {
        snakes[i].body = snakes[i-1].body;
    }

    snakes[0].body.x += snakes[0].dx;
    snakes[0].body.y += snakes[0].dy;

    // snake rende
    SDL_SetRenderDrawColor(rdr, 0, 255, 0, 255);
    for (snake_body snake : snakes)
    {
        SDL_RenderFillRect(rdr, &snake.body);
    }

    SDL_RenderPresent(rdr);

    return 0;

}

int main()
{
    Init();
    SDL_Event event;

    while (true)
    {
        // update screen
        Draw();

        // input detection
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                // Destroy, free RAM
                SDL_DestroyTexture(bg_texture);
                SDL_DestroyRenderer(rdr);
                SDL_DestroyWindow(win);
                SDL_Quit();
                IMG_Quit();

                return 0;
            }
            if (event.type ==SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        snakes[0].dx = 0;
                        snakes[0].dy = -(OBJ_WIDTH+5);
                        break;

                    case SDLK_DOWN:
                        snakes[0].dx = 0;
                        snakes[0].dy = OBJ_WIDTH+5;
                        break;

                    case SDLK_LEFT:
                        snakes[0].dx = -(OBJ_WIDTH+5);
                        snakes[0].dy = 0;
                        break;

                    case SDLK_RIGHT:
                        snakes[0].dx = OBJ_WIDTH+5;
                        snakes[0].dy = 0;
                        break;
                }
            }

        }

        // check events
        if (boundary_check(snakes))
        {
            break;
        }

        SDL_Delay(100);
    }

    // Destroy, free RAM
    SDL_DestroyTexture(bg_texture);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
    IMG_Quit();

}