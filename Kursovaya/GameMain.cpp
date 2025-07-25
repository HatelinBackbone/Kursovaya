#include "Functions.h"
#include "Menu.h"
#include "lvl_select.h"
#include "Level_1.h"
#include "Level_2.h"
#include "Level_3.h"

const int SCREEN_WIDTH = 1296;
const int SCREEN_HEIGTH = 720;

int main(int argc, char** argv)
{
	system("chcp 1251");

	SDL_Window* win = NULL;
	SDL_Renderer* ren = NULL;
	SDL_Surface* surf = NULL;

	bool men = true;
	bool menurun = true;
	bool sel = false;
	bool run_1 = false;
	bool run_2 = false;
	bool run_3 = false;
	bool act = true;
	bool playing = true;

	int s = 0;
	int sw;

	init();//инициализация библиотек

	win = SDL_CreateWindow("I HATE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGTH, 0);//создание окна
	if (win == NULL)
	{
		const char* s = SDL_GetError();
		printf_s("ќшибка создани¤ окна %s\n", s);
		return 5;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);//создание рендерера
	if (ren == NULL)
	{
		const char* s = SDL_GetError();
		printf_s("ќшибка создани¤ рендерера %s\n", s);
		return 6;
	}

	Mix_Music* music = NULL;//звуки игры
	Mix_Chunk* click = NULL;
	Mix_Chunk* door = NULL;
	click = Mix_LoadWAV("Click.wav");
	door = Mix_LoadWAV("Door_open.wav");

	music = Mix_LoadMUS("music.wav");//музыка
	if (music == NULL)
	{
		printf("ќшибка загрузки музыки\n");
		return 1;
	}
	Mix_PlayMusic(music, -1);

	while (men)
	{
		while (act)
		{
			sw = menu(ren, menurun);//начало игры с запуска меню
			switch (sw)
			{
			case 1://начать игру, вход в меню выбора уровней
				Mix_PlayChannel(-1, click, 0);
				sel = true;
				menurun = false;
				act = false;
				break;
			case 2://настройки
				if (playing)
				{
					Mix_PlayChannel(-1, click, 0);
					Mix_PauseMusic();
					playing = false;
				}
				else
				{
					Mix_PlayChannel(-1, click, 0);
					Mix_ResumeMusic();
					playing = true;
				}
				break;
			case 3://выход из игры
				Mix_PlayChannel(-1, click, 0);
				Mix_FreeChunk(door);
				Mix_FreeChunk(click);
				Mix_FreeMusic(music);
				act = false;
				Quit(ren, win);
				return 0;
				break;
			}
		}

		if (sel)//если выбрали начать игру, открытие меню выбора уровней
			s = lvl_sel(ren, sel);

		switch (s)
		{
		case 0://выход из игры
			Mix_PlayChannel(-1, click, 0);
			Mix_FreeChunk(door);
			Mix_FreeChunk(click);
			Mix_FreeMusic(music);
			Quit(ren, win);
			return 0;
		case -1://вернутьс¤ в главное меню
			Mix_PlayChannel(-1, click, 0);
			sel = false;
			menurun = true;
			act = true;
			break;
		case 1://запуск 1го уровн¤
			Mix_PlayChannel(-1, click, 0);
			run_1 = true;
			if (lvl_1(ren, run_1) == 1)
				Mix_PlayChannel(1, door, 0); 
			break;
		case 2://запуск 2го уровн¤
			Mix_PlayChannel(-1, click, 0);
			run_2 = true;
			if (lvl_2(ren, run_2) == 1)
				Mix_PlayChannel(1, door, 0);
			break;
		case 3://запуск 3го уровн¤
			Mix_PlayChannel(-1, click, 0);
			run_3 = true;
			lvl_3(ren, run_3);
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		} 
	}
}