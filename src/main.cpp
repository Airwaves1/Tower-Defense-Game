#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <json.hpp>

#ifdef PROJECT_DIR
#define ASSET_DIR PROJECT_DIR "/install/resources/"
#else
#define ASSET_DIR "./resources/"
#endif

const int FPS = 60;

int main()
{
    std::cout << "Hello, Tower Defense!" << std::endl;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    const char* path = ASSET_DIR "ui_home_avatar.png";
    SDL_Surface *suf_img = IMG_Load(path);
    SDL_Texture *tex_img = SDL_CreateTextureFromSurface(renderer, suf_img);

    bool is_quit = false;

    SDL_Event event;
    SDL_Point pos_cursor = {0, 0};
    SDL_Rect rect_img;

    uint64_t last_counter = SDL_GetPerformanceCounter();
    uint64_t conter_freq = SDL_GetPerformanceFrequency();

    rect_img.w = suf_img->w;
    rect_img.h = suf_img->h;

    while (!is_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                is_quit = true;
            }else if (event.type == SDL_MOUSEMOTION)
            {
                pos_cursor.x = event.motion.x;
                pos_cursor.y = event.motion.y;
            }
        }

        uint64_t current_counter = SDL_GetPerformanceCounter();
        double delta = (current_counter - last_counter) / (double)conter_freq;
        last_counter = current_counter;
        if(delta * 1000 < 1000.0 / 60.0){
            SDL_Delay(uint32_t(1000.0 / 60.0 - delta * 1000));
        }

        // 处理数据
        rect_img.x = pos_cursor.x - rect_img.w / 2;
        rect_img.y = pos_cursor.y - rect_img.h / 2;

        // 渲染
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, tex_img, NULL, &rect_img);
        SDL_RenderPresent(renderer);
    }
}
