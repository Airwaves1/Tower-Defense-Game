#pragma once
#include "manager.hpp"
#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <chrono>

namespace TD
{
struct ApplicationConfig
{
    uint16_t window_width    = 1200;
    uint16_t window_height   = 800;
    const char *window_title = "Tower Defense";
};

class GameManager : public Manager<GameManager>
{
    friend class Manager<GameManager>;

  public:
    void init(const ApplicationConfig &config)
    {
        m_config = config;
        init_assert(!SDL_Init(SDL_INIT_EVERYTHING), "SDL initialization failed");
        init_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG), "SDL_image initialization failed");
        init_assert(!TTF_Init(), "SDL_ttf initialization failed");
        init_assert(Mix_Init(MIX_INIT_MP3), "SDL_mixer initialization failed");

        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

        m_window =
            SDL_CreateWindow(m_config.window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             m_config.window_width, m_config.window_height, SDL_WINDOW_SHOWN);
        init_assert(m_window, "Window creation failed");

        m_renderer = SDL_CreateRenderer(m_window, -1,
                                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
                                            SDL_RENDERER_TARGETTEXTURE);
        init_assert(m_renderer, "Renderer creation failed");

        m_start_time = std::chrono::high_resolution_clock::now();
    }

    int run()
    {
        while (m_running)
        {
            m_current_time = std::chrono::high_resolution_clock::now();
            m_delta_time   = std::chrono::duration<float>(m_current_time - m_last_time).count();
            m_last_time    = m_current_time;
            std::cout << m_delta_time << std::endl;

            while (SDL_PollEvent(&m_event)) onInit();

            // 锁定帧率至 60 FPS
            float frame_time = 1.0f / m_fps;
            if (m_delta_time < frame_time)
            {
                SDL_Delay((frame_time - m_delta_time) * 1000);
            }

            onUpdate(m_delta_time);
            onRender();
        }

        return 0;
    }

    void onInit()
    {
        if (m_event.type == SDL_QUIT) m_running = false;
    }

    void onUpdate(float delta_time) {}

    void onRender() {}

  protected:
    GameManager() = default;
    virtual ~GameManager()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        Mix_CloseAudio();
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

  private:
    void init_assert(bool flag, const char *err_msg)
    {
        if (flag) return;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", err_msg, m_window);
        exit(-1);
    }

  private:
    ApplicationConfig m_config;
    SDL_Window *m_window     = nullptr;
    SDL_Renderer *m_renderer = nullptr;

    SDL_Event m_event;

    bool m_running = true;

    uint16_t m_fps     = 60;
    float m_delta_time = 0.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_current_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_time;
};

} // namespace TD
