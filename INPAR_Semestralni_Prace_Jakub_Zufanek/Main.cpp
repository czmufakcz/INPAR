#include <stdio.h>
#include <omp.h>
#include "SDL.h"
#include <list>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool quit = false;
SDL_Event in;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Line
{
	double pointX, pointY, length, angle;

	Line(double x, double y, double length, double angle) : pointX(x), pointY(y), length(length), angle(angle) {}

	double getX2()
	{
		return pointX + cos(angle*(M_PI / 180.0))*length;
	}

	double getY2()
	{
		return pointY + sin(angle*(M_PI / 180.0))*length;
	}

	void draw()
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLine(renderer, pointX, pointY, getX2(), getY2());
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Resetting the color
	}
};

std::list<Line*> lines;

void kochFractal(std::list<Line*> & lines)
{
	std::list<Line*> newLines;
	std::list<Line*> delLines;

	for (auto itr = lines.begin(); itr != lines.end(); itr++)
	{
		double x_l1 = (*itr)->pointX;
		double y_l1 = (*itr)->pointY;
		double len_l1 = (*itr)->length / 3;
		double ang_l1 = (*itr)->angle;

		double x_l2 = (*itr)->pointX + (cos((*itr)->angle*(M_PI / 180.0))*(*itr)->length / 1.5);
		double y_l2 = (*itr)->pointY + (sin((*itr)->angle*(M_PI / 180.0))*(*itr)->length / 1.5);
		double len_l2 = (*itr)->length / 3;
		double ang_l2 = (*itr)->angle;

		double x_l3 = (*itr)->pointX + (cos((*itr)->angle*(M_PI / 180.0))*(*itr)->length / 3.0);
		double y_l3 = (*itr)->pointY + (sin((*itr)->angle*(M_PI / 180.0))*(*itr)->length / 3.0);
		double len_l3 = (*itr)->length / 3.0;
		double ang_l3 = (*itr)->angle - 300.0;

		double x_l4 = (*itr)->pointX + (cos((*itr)->angle*(M_PI / 180.0))*((*itr)->length / 1.5));
		double y_l4 = (*itr)->pointY + (sin((*itr)->angle*(M_PI / 180.0))*((*itr)->length / 1.5));
		double len_l4 = (*itr)->length / 3.0;
		double ang_l4 = (*itr)->angle - 240.0;

		x_l4 = x_l4 + cos(ang_l4*(M_PI / 180.0))*len_l4;
		y_l4 = y_l4 + sin(ang_l4*(M_PI / 180.0))*len_l4;
		ang_l4 -= 180.0;

		newLines.push_back(new Line(x_l1, y_l1, len_l1, ang_l1));
		newLines.push_back(new Line(x_l2, y_l2, len_l2, ang_l2));
		newLines.push_back(new Line(x_l3, y_l3, len_l3, ang_l3));
		newLines.push_back(new Line(x_l4, y_l4, len_l4, ang_l4));

		delLines.push_back((*itr));
	}

	for (auto itr = newLines.begin(); itr != newLines.end(); itr++)
		lines.push_back((*itr));

	for (auto itr = delLines.begin(); itr != delLines.end(); itr++)
	{
		lines.remove((*itr));
		delete (*itr);
	}
}

int main(int argc, char** args)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("INPAR - Kochova vlocka", SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	lines.push_back(new Line(WINDOW_WIDTH - 200, 150, WINDOW_WIDTH - 400, 180.0));
	lines.push_back(new Line(200, 150, WINDOW_WIDTH - 400, 60.0));
	Line* lineS = new Line(WINDOW_WIDTH - 200, 150, WINDOW_WIDTH - 400, 120.0);
	lineS->pointX += cos(lineS->angle *(M_PI / 180.0))*lineS->length;
	lineS->pointY += sin(lineS->angle *(M_PI / 180.0))*lineS->length;
	lineS->angle -= 180.0;
	lines.push_back(lineS);
	signed int countIteration = 0;

	while (!quit)
	{

		while (SDL_PollEvent(&in))
		{
			if (in.type == SDL_QUIT)
				quit = true;
		}

		SDL_RenderClear(renderer); 

		for (auto itr = lines.begin(); itr != lines.end(); itr++)
			(*itr)->draw();

		SDL_RenderPresent(renderer);

		SDL_Delay(1000); 
		kochFractal(lines); 
		countIteration++;
		printf("Count iteration: %d \n", countIteration);
	}

	for (auto itr = lines.begin(); itr != lines.end(); itr++)
		delete (*itr); 

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}