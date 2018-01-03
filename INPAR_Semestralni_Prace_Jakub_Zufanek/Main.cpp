#include <stdio.h>
#include <omp.h>
//#include "SDL.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <list>
#include <vector>

#define COUNT_THREADS 1
#define KOCH_CONSTANT 4
/*
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event in;
*/
bool quit = false;

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
	/*
	void draw()
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLine(renderer, pointX, pointY, getX2(), getY2());
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}*/
};



int main(int argc, char** args)
{/*
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("INPAR - Kochova vlocka", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);*/

	std::vector<Line*> lines;
	std::vector<Line*> newLines;
	//INIT LINES AND PUSH TO VECTOR
	lines.push_back(new Line(WINDOW_WIDTH - 200, 150, WINDOW_WIDTH - 400, 180.0));
	lines.push_back(new Line(200, 150, WINDOW_WIDTH - 400, 60.0));
	Line* lineS = new Line(WINDOW_WIDTH - 200, 150, WINDOW_WIDTH - 400, 120.0);
	lineS->pointX += cos(lineS->angle *(M_PI / 180.0))*lineS->length;
	lineS->pointY += sin(lineS->angle *(M_PI / 180.0))*lineS->length;
	lineS->angle -= 180.0;
	lines.push_back(lineS);

	//COUNT ITERATION
	signed int countIteration = 0;
	//TIME MEASUREMENT
	double start, end;
	start = omp_get_wtime();
	//VARIABLES FOR EACH THREAD
	Line* line = NULL;
	double pointX = 0;
	double pointY = 0;
	double length = 0;
	double angle = 0;

	while (!quit)
	{

		/*
		while (SDL_PollEvent(&in))
		{
			if (in.type == SDL_QUIT)
				quit = true;
		}

		SDL_RenderClear(renderer);

		//Draw lines through SDL
		for (int  i = 0; i < lines.size(); i++)
		{
			lines[i]->draw();
		}

		SDL_RenderPresent(renderer);
	*/
	
		//SIZES FOR TIMES RUNNING
		const int size = lines.size();
		//FORWARD ALLOCATE FOR NEW ITERATES
		newLines.clear();
		newLines.reserve(lines.size() * KOCH_CONSTANT);
		//FORWARD ALLOCATE FOR EACH THREAD
		const int sizeThreadVectorLines = std::ceil(size / COUNT_THREADS);
		std::vector<Line*> threadLines = std::vector<Line*>(sizeThreadVectorLines);

		//Issue with vector which is not thread safe....3 threads are better usage for longer run
#pragma omp parallel num_threads(COUNT_THREADS) shared(lines, newLines) private(line, pointX, pointY, length, angle,threadLines) 
		{
#pragma omp for 
			for (int i = 0; i < size; i++) {
				line = lines[i];
				lines[i] = NULL;

				pointX = line->pointX;
				pointY = line->pointY;
				length = line->length;
				angle = line->angle;
				//CALCULATE FIRST LINE
				double x_l1 = pointX;
				double y_l1 = pointY;
				double len_l1 = length / 3;
				double ang_l1 = angle;
				//CALCULATE SECOND LINE
				double x_l2 = pointX + (cos(angle*(M_PI / 180.0))*length / 1.5);
				double y_l2 = pointY + (sin(angle*(M_PI / 180.0))*length / 1.5);
				double len_l2 = length / 3;
				double ang_l2 = angle;
				//CALCULATE THIRD LINE
				double x_l3 = pointX + (cos(angle*(M_PI / 180.0))*length / 3.0);
				double y_l3 = pointY + (sin(angle*(M_PI / 180.0))*length / 3.0);
				double len_l3 = length / 3.0;
				double ang_l3 = angle - 300.0;
				//CALCULATE FOURTH LINE
				double x_l4 = pointX + (cos(angle*(M_PI / 180.0))*(length / 1.5));
				double y_l4 = pointY + (sin(angle*(M_PI / 180.0))*(length / 1.5));
				double len_l4 = length / 3.0;
				double ang_l4 = angle - 240.0;
				x_l4 = x_l4 + cos(ang_l4*(M_PI / 180.0))*len_l4;
				y_l4 = y_l4 + sin(ang_l4*(M_PI / 180.0))*len_l4;
				ang_l4 -= 180.0;

				//PUSH NEW LINES TO VECTOR
				threadLines.push_back(new Line(x_l1, y_l1, len_l1, ang_l1));
				threadLines.push_back(new Line(x_l2, y_l2, len_l2, ang_l2));
				threadLines.push_back(new Line(x_l3, y_l3, len_l3, ang_l3));
				threadLines.push_back(new Line(x_l4, y_l4, len_l4, ang_l4));

				//DEALOCATE LINE
				delete line;
			}
#pragma omp critical
			{
				//COMPOSITION FOR MAIN VECTOR
				for (auto tmp : threadLines) {
					newLines.push_back(tmp);
				}
			}

		}
		lines = newLines;

		//INFORMATION IN CONSOLE
		countIteration++;
		printf("Count iteration: %d \n", countIteration);
		printf("Count state: %d \n", lines.size());
		end = omp_get_wtime();
		printf("Time duration %f seconds.\n", end - start);
		printf("\n");
	}
	//DEALOCATE LINE
	for (auto line : lines) {
		delete line;
	}
	//Quit SDL
	/*
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	*/
	return 0;
}