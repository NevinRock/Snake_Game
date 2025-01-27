#include <algorithm>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <random>
#include <cmath>

// global define
#define OBJ_WIDTH 70
#define OBJ_HEIGHT 70
#define BODY_WIDTH 20
#define BODY_HEIGHT 20

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int POINT_COUNT = 0;
int Food_Frame_Count = 0;
const int FPS = 60;
const int FrameDelay = 1000 / FPS;

SDL_Window *win = nullptr;
SDL_Renderer *rdr = nullptr;
SDL_Surface *bg = nullptr;
SDL_Texture *bg_texture = nullptr;
Mix_Music *bg_music = nullptr;

SDL_Texture *food_texture = nullptr;
SDL_Texture *band_texture = nullptr;


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

// food
std::vector<SDL_Texture*> food_imgs;

void load_food_images(SDL_Renderer* rdr)
{
    for (int i = 0; i < 30; i++)
    {
        std::string img_path = "../media/food/" + std::to_string(i) + ".png";
        SDL_Texture* imgs = IMG_LoadTexture(rdr, img_path.c_str());
        if (imgs == nullptr)
        {
            std::cerr << "Failed to load food images" << std::endl;
        }

        food_imgs.push_back(imgs);
    }
}

// overlap checking
template <typename T>
int overlap_check(const std::vector<snake_body> &snake , const std::vector<T> &obj)
{
    SDL_bool output = SDL_FALSE;

    for (int i = 0; i < snake.size(); i++)
    {
        for (int j = 0; j < obj.size(); j++)
        {
            if (SDL_HasIntersection(&snake[i].body, &obj[j].body))
            {
                return static_cast<int>(j);
            }
        }
    }

    return -1;
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
    // global init
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }


    win = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
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

    bg_music = Mix_LoadMUS("../media/background.mp3");
    if (bg_music == nullptr) {
        std::cout << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    // play music
    Mix_PlayMusic(bg_music, -1);

    // band init
    band_texture = IMG_LoadTexture(rdr, "../media/band.jpg");

    //food init
    load_food_images(rdr);




    //snake init
    for (int i = 0; i < 4; i++)
    {
        snakes.push_back({{200-(BODY_WIDTH+5)*i, 200, BODY_WIDTH,BODY_HEIGHT}, 5, 5});
    }

    return 0;
}

int Draw()
{
    SDL_RenderClear(rdr);
    SDL_RenderCopy(rdr, bg_texture, NULL, NULL);

    //snake movement
    for (int i = snakes.size()-1; i > 0; i--)
    {
        snakes[i].body = snakes[i-1].body;
    }

    snakes[0].body.x += snakes[0].dx;
    snakes[0].body.y += snakes[0].dy;

    // snake rende

    for (int i = 0; i < snakes.size(); i++)
    {
        if (i == 0)
        {
            SDL_SetRenderDrawColor(rdr, 0, 255, 255, 255);
        }else
        {
            SDL_SetRenderDrawColor(rdr, 0, 255, 0, 100);
        }
        SDL_RenderFillRect(rdr, &snakes[i].body);
    }

    // food rende
    for (food_location &food : foods)
    {
        food_texture = food_imgs[Food_Frame_Count];
        SDL_RenderCopy(rdr, food_texture, NULL, &food.body);
        Food_Frame_Count = (Food_Frame_Count + 1) % 30;

    }

    // band rende
    for (band_location &band : bands)
    {
        SDL_RenderCopy(rdr, band_texture, NULL, &band.body);
    }


    SDL_RenderPresent(rdr);

    return 0;

}

int OBJ_Add()
{
    int add_food_num = std::max(2*POINT_COUNT, 1);
    int add_band_num = std::max(3*POINT_COUNT, 1);

    // random init
    std::random_device rd; // 用于生成种子
    std::mt19937 gen(rd()); // 随机数生成器（Mersenne Twister）
    std::uniform_int_distribution<> dist_x(25, WINDOW_WIDTH - 25);
    std::uniform_int_distribution<> dist_y(0, WINDOW_HEIGHT - 25);


    // add food
    for (int i = 0; i < add_food_num; i++)
    {
        foods.push_back({{dist_x(gen), dist_y(gen), OBJ_WIDTH, OBJ_WIDTH}});
    }

    // add band
    for (int i = 0; i < add_band_num; i++)
    {
        bands.push_back({{dist_x(gen), dist_y(gen), OBJ_WIDTH, OBJ_WIDTH}});
    }

    // check overlap between foods and bands
    for (int i = 0; i < foods.size(); i++)
    {
        for (int j = 0; j < bands.size(); j++)
        {
            if (foods[i].body.x == bands[j].body.x && foods[i].body.y == bands[j].body.y)
            {
                bands.erase(bands.begin() + j);
            }
        }
    }

    return 0;
}

int snake_body_add()
{
    snake_body snakes_new_body = snakes.back();
    snakes.push_back(snakes_new_body);

    return 0;
}

int main()
{
    int loop_cout = 0;
    Init();
    OBJ_Add();

    SDL_Event event;

    while (true)
    {
        loop_cout++;
        if (loop_cout % 100 == 0)
        {
            OBJ_Add();
        }

        // update screen
        Draw();

        // 获取帧开始时间
        Uint32 frame_start = SDL_GetTicks();

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
                    case SDLK_e:
                        return 1;

                    default:
                        break;
                }
            }
            if (event.type == SDL_MOUSEMOTION) // 检测鼠标移动事件
            {
                int mouse_x = event.motion.x; // 鼠标的 X 坐标
                int mouse_y = event.motion.y; // 鼠标的 Y 坐标
                // std::cout << "Mouse Position: (" << mouse_x << ", " << mouse_y << ")" << std::endl;
                // update snake position
                double delta_x = snakes[0].body.x - mouse_x;
                double delta_y = snakes[0].body.y - mouse_y;
                double theta = atan2(delta_y, delta_x);
                std::cout << "theta: " << theta << std::endl;

                double dx = snakes[0].dx * cos(theta);
                double dy = snakes[0].dy * sin(theta);


                snakes[0].body.x -= dx;
                snakes[0].body.y -= dy;

                std::cout << "snake_x: " << snakes[0].body.x << "  snake_y: " << snakes[0].body.y << std::endl;
            }




            // reshape window
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                WINDOW_WIDTH = event.window.data1; // update new width
                WINDOW_HEIGHT = event.window.data2; // update new height

                std::cout << "Window resized: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
            }

        }

        // check events

        // boundary_check
        if (boundary_check(snakes))
        {
            std::cout << "Collition Boundary" << std::endl;
            break;
        }

        SDL_Delay(100);

        // food_check
        int overlap_check_food = overlap_check(snakes, foods);
        if (overlap_check_food != -1)
        {
            std::cout << "overlap with food" << std::endl;
            foods.erase(foods.begin()+overlap_check_food);
            snake_body_add();
            OBJ_Add();
            POINT_COUNT++;
        }

        // band check
        int overlap_check_band = overlap_check(snakes, bands);
        if (overlap_check_band != -1)
        {
            std::cout << "overlap with bands" << std::endl;
            break;
        }

        // 限制帧率
        Uint32 frame_time = SDL_GetTicks() - frame_start; // 计算帧耗时
        if (frame_time < FrameDelay)
        {
            SDL_Delay(FrameDelay - frame_time); // 等待剩余时间
        }


    }




    // Destroy, free RAM
    SDL_DestroyTexture(bg_texture);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
    IMG_Quit();

}