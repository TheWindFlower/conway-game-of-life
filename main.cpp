#include <iostream>
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include <vector>
#include <tuple>
#include <string>
#include <sstream>
#include <future>
#include "include/util.hpp"

using std::tuple;
using std::vector;

// default value
int SCREEN_WIDTH = 700;
int SCREEN_HEIGHT = 700;
int GRID_SIZE = 2; // 5
int ROWS = SCREEN_HEIGHT / GRID_SIZE;
int COLUMNS = SCREEN_WIDTH / GRID_SIZE;

// Function to change the color of some of the grid cells to white
class render
{
private:
    void draw_cells(SDL_Renderer *renderer, vector<tuple<int, int, bool>> cells, int rows, int columns)
    {
        int x, y;
        bool status;
        for (const auto &t : cells)
        {
            x = std::get<0>(t);
            y = std::get<1>(t);
            status = std::get<2>(t);
            SDL_Rect rect = {x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
            if (status)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }

public:
    int sdl(int delay)
    {

        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Create window
        window = SDL_CreateWindow("conway game of life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); //  SDL_WINDOW_FULLSCREEN_DESKTOP

        if (window == NULL)
        {
            std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
        {
            std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        vector<tuple<int, int, bool>> cells_status = start("data/board.brd", ROWS, COLUMNS); // load initale game stat

        // Wait for user to quit
        bool quit = false;
        SDL_Event event;

        // Create a promise object
        std::promise<vector<tuple<int, int, bool>>> resultPromise;

        // Get a future object from the promise
        std::future<vector<tuple<int, int, bool>>> resultFuture = resultPromise.get_future();

        // Create a thread and pass the promise object to the function
        std::thread myThread(next_generation, std::ref(resultPromise), cells_status, ROWS, COLUMNS);

        // main loop
        while (!quit)
        {
            auto start = std::chrono::high_resolution_clock::now();

            // Draw grid
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
            for (int i = 0; i < ROWS; i++)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    SDL_Rect rect = {j * GRID_SIZE, i * GRID_SIZE, GRID_SIZE, GRID_SIZE};
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }

            // Change color of some cells to white
            draw_cells(renderer, cells_status, ROWS, COLUMNS);
            // Update screen
            SDL_RenderPresent(renderer);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            // calc of the next game status
            auto start_calc = std::chrono::high_resolution_clock::now();
            myThread = std::thread(next_generation, std::ref(resultPromise), cells_status, ROWS, COLUMNS);
            myThread.join();
            cells_status = resultFuture.get();
            // cells_status = next_generation(cells_status, ROWS, COLUMNS);
            auto end_calc = std::chrono::high_resolution_clock::now();
            auto duration_calc = std::chrono::duration_cast<std::chrono::microseconds>(end_calc - start_calc);
            std::cout << "Calc execution time: " << duration_calc.count() << " microseconds." << std::endl;

            // handle event
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "Frame render time: " << duration.count() << " microseconds." << std::endl;
        }

        // Clean up
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }
};
int main(int argc, char *argv[])
{
    int delay = 0;
    bool args = false;
    bool gen = false;
    for (int i = 0; i < argc; i++) // loop over each given arguments
    {
        char *instr = argv[i];

        if (strcmp(instr, "-r") == 0 || strcmp(instr, "--regen") == 0)
        {
            gen = true;
        }
        if (strcmp(instr, "-l") == 0 || strcmp(instr, "--load") == 0)
        {
            continue;
        }
        if (strcmp(instr, "-w") == 0 || strcmp(instr, "--width") == 0)
        {
            args = true;
            std::string arg = argv[i + 1];
            SCREEN_WIDTH = std::stoi(arg);
        }
        if (strcmp(instr, "-h") == 0 || strcmp(instr, "--height") == 0)
        {
            args = true;
            std::string arg = argv[i + 1];
            SCREEN_HEIGHT = std::stoi(arg);
        }
        if (strcmp(instr, "-s") == 0 || strcmp(instr, "--size") == 0)
        {
            // TODO aint working yet
            int s, g;
            std::string s_ = argv[s + 1];
            std::string g_ = argv[g + 2];
            s = std::stoi(s_);
            g = std::stoi(g_);
            SCREEN_HEIGHT = s;
            SCREEN_WIDTH = s;
            GRID_SIZE = g;
        }
        if (strcmp(instr, "-g") == 0 || strcmp(instr, "--grid_size") == 0)
        {
            args = true;
            std::string arg = argv[i + 1];
            GRID_SIZE = std::stoi(arg);
        }
        if (strcmp(instr, "-f") == 0 || strcmp(instr, "--fps") == 0)
        {
            std::string arg = argv[i + 1];
            delay = std::stoi(arg);
        }
    }
    ROWS = SCREEN_HEIGHT / GRID_SIZE;
    COLUMNS = SCREEN_WIDTH / GRID_SIZE;
    if (gen)
    {
        grid_gen(ROWS, COLUMNS);
        render rd;
        rd.sdl(delay);
    }
}
