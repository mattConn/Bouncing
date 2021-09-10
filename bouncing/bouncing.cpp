#include <iostream>
#include<algorithm>
#include<vector>
#include <map>
#include<windows.h>

#include <SDL.h>
#undef main

using namespace std;

#define WINDOW_WIDTH 900

class Rect : public SDL_Rect
{
public:
	int xDir, yDir, xVel, yVel, red, green, blue;
	Rect()
	{
		// width, height
		w = h = rand() % 30 + 10;
		x = y = rand() % WINDOW_WIDTH;
		xDir = yDir = xVel = yVel = 1;

		// direction, velocity
		xDir *= (rand() % 1 == 0 ? -1 : 1);
		xVel = rand() % 10 + 1;

		yDir *= (rand() % 1 == 1 ? -1 : 1);
		yVel = rand() % 10 + 1;

		// color
		red = rand() % 255 + 1;
		green = rand() % 255 + 1;
		blue = rand() % 255 + 1;

		// outlier
		if (rand() % 20 == 2)
		{
			xDir *= 2;
			yDir *= 2;
		}
	}
};

int main(void) {

	srand(time(NULL));
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "Bouncing");

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // canvas

	int minRects = 5;

	vector<Rect> rects(minRects);

	bool running = true;
	int randomColors = 0;
	int leaveTrails = 0;
	bool keyDown = false;
	int pause = 0;
	int minVel = 1;

	while (running)
	{

		const Uint8* keystate = SDL_GetKeyboardState(NULL);

		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
				break;
			}

			if (keystate[SDL_SCANCODE_BACKSPACE]) // Reset
			{
				rects.clear();
				for (int i = 0; i < minRects; i++) rects.push_back(Rect());
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_R]) // Random colors and trails
			{
				randomColors ^= 1;
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_UP]) // Flock up
			{
				for (auto& r : rects)
				{
					r.yDir = -1;
					r.xVel = 1;
				}
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_DOWN]) // Flock down
			{
				for (auto& r : rects)
				{
					r.yDir = 1;
					r.xVel = 1;
				}
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_LEFT]) // Flock left
			{
				for (auto& r : rects)
				{
					r.xDir = -1;
					r.yVel = 1;
				}
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_RIGHT]) // Flock right
			{
				for (auto& r : rects)
				{
					r.xDir = 1;
					r.yVel = 1;
				}
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_EQUALS]) // Add rects
			{
				rects.push_back(Rect());
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_MINUS]) // Remove rects
			{
				if(!rects.empty())
					rects.pop_back();
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_LEFTBRACKET]) // Slow down
			{
				minVel = minVel <= 0 ? 1 : minVel-1;
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_RIGHTBRACKET]) // Speed up
			{
				minVel++;
				keyDown = true;
			}
			else if (!keyDown && keystate[SDL_SCANCODE_ESCAPE]) // Pause
			{
				pause ^= 1;
				keyDown = true;
			}
			else keyDown = false;
		}


		for (auto& r : rects)
		{
			if (pause || rects.empty()) goto render;
			if (r.x + r.w > WINDOW_WIDTH || r.x < 0)
			{
				r.xDir *= -1;
				r.xVel = rand() % 10 + 1;

				if (r.x + r.w > WINDOW_WIDTH) r.x = WINDOW_WIDTH - r.w;
				else if (r.x < 0) r.x = 0;
			}
			if (r.y + r.h > WINDOW_WIDTH || r.y < 0)
			{
				r.yDir *= -1;
				r.yVel = rand() % 10 + 1;

				if (r.y + r.h > WINDOW_WIDTH) r.y = WINDOW_WIDTH - r.h;
				else if (r.y < 0) r.y = 0;
			}

			r.x += r.xVel * r.xDir * minVel;
			r.y += r.yVel * r.yDir * minVel;


			render:
			if(randomColors && !pause)
				SDL_SetRenderDrawColor(renderer, rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1, 255); // random color 
			else
				SDL_SetRenderDrawColor(renderer, r.red, r.green,  r.blue, 255); // white 

			SDL_RenderFillRect(renderer, &r);
		}

		SDL_RenderPresent(renderer);

		SDL_SetRenderDrawColor(renderer, 0,0,0, 255);

		if(!randomColors) SDL_RenderClear(renderer);

		SDL_Delay(16);

	} // end while

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}