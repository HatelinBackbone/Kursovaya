#include "Functions.h"

struct menu_but//структура для хранения надписей на кнопках
{
	char but1[32];
	char but2[32];
	char but3[32];
	char NAME[32];
};

int menu_but_rec(const char file[], menu_but* menu)//чтение из файла в структуру надписей на кнопках
{
	FILE* f;
	if (fopen_s(&f, file, "r+") != 0)
	{
		printf_s("Не удалось открыть файл\n");
		return 1;
	}

	char buffer[32];
	char* token = NULL;
	char* next_token = NULL;

	fgets(buffer, 32, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->but1,32, token);
	buffer[0] = '\0';

	fgets(buffer, 32, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->but2,32, token);
	buffer[0] = '\0';

	fgets(buffer, 32, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->but3,32, token);
	buffer[0] = '\0';

	fgets(buffer, 32, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->NAME, 32, token);
	buffer[0] = '\0';

	fclose(f);
	return 0;
}

void exit_menu(TTF_Font* txt, SDL_Texture* button, SDL_Texture* start, SDL_Texture* settings, SDL_Texture* exit, SDL_Texture* persStand, 
	SDL_Texture* brick, SDL_Texture* name)
{
	TTF_CloseFont(txt);
	SDL_DestroyTexture(button);
	SDL_DestroyTexture(start);
	SDL_DestroyTexture(settings);
	SDL_DestroyTexture(exit);
	SDL_DestroyTexture(persStand);
	SDL_DestroyTexture(brick);
	SDL_DestroyTexture(name);
}

int menu(SDL_Renderer* ren, bool run)//главное меню игры
{
	menu_but menub = { NULL,NULL,NULL };
	SDL_Event ev;
	SDL_Rect dstName = {200,50,500,100}, srcButton = { 0,0,0,0 }, dstButton = { 0,0,0,0 }, srcRectBrick = { 0,0,0,0 }, dstRectBrick = { 170,400,216,216 }, srcRectPers = { 0,0,0,0 }, dstRectPers = { 100,160,330,240 }, clip = { 0, 0, 110, 80 };
	SDL_Texture* button = NULL, *start = NULL, *settings = NULL, *exit = NULL, *name = NULL, *persStand = NULL, *brick = NULL;
	TTF_Font* txt = NULL;
	SDL_Color txtc = { 0, 0, 0, 0 }; // цвет текста
		
	float dt;
	float frameTime = 0.03f;
	float animTime = 0.0f;

	int ot = SDL_GetTicks(), nt;
	int n = 0, a = 0;

	button = load("Button_long.png", &srcButton, ren, button);//кнопки в меню

	txt = TTF_OpenFont("font.ttf", 100);//открытие шрифта для меню


	menu_but_rec("Menu_Buttons.txt", &menub);
	start = create_text_texture(ren, menub.but1, txt, txtc, &dstButton);//создание текстовых текстур для кнопок
	settings = create_text_texture(ren, menub.but2, txt, txtc, &dstButton);
	exit = create_text_texture(ren, menub.but3, txt, txtc, &dstButton);
	name = create_text_texture(ren, menub.NAME, txt, txtc, &dstName);//создание текстовой текстуры названия игры

	persStand = load("Sprites/StandSprites.png", &srcRectPers, ren, persStand);
	brick = load("KenneY/PNG/brick.png", &srcRectBrick, ren, brick);

	while (run)
	{
		nt = SDL_GetTicks();
		dt = (nt - ot) / 1000.0f;
		ot = nt;

		while (SDL_PollEvent(&ev))//обработка событий
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				exit_menu(txt, button, start, settings, exit, persStand, brick, name);
				return 3;
				break;
			case SDL_WINDOWEVENT:
				ot = SDL_GetTicks();
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (ev.button.button)
				{
				case SDL_BUTTON_LEFT:
					int x = ev.button.x;
					int y = ev.button.y;
					if (x >= 700 && x <= 1080)//проверка попадания клика в кнопки
					{
						if (y >= 200 && y <= 300)//для 1ой кнопки(старт игры)
						{
							exit_menu(txt, button, start, settings, exit, persStand, brick, name);
							return 1;
						}
						if (y >= 320 && y <= 420)//настройки
						{
							exit_menu(txt, button, start, settings, exit, persStand, brick, name);
							return 2;
						}
						if (y >= 440 && y <= 540)//выход из игры
						{
							exit_menu(txt, button, start, settings, exit, persStand, brick, name);
							return 3;
						}
					}
						break;
				}
			}
		}

		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); //фон 
		SDL_RenderClear(ren);

		SDL_RenderCopy(ren, name, NULL, &dstName);

		for (int i = 0; i < 3; i++)//отрисовка кнопок с текстом
		{
			dstButton = { 700, 200 + i * (100 + 20),380,100 };
			SDL_RenderCopy(ren, button, NULL, &dstButton);
			switch (i)
			{
			case 0:
				SDL_RenderCopy(ren, start, NULL, &dstButton);
				break;
			case 1:
				SDL_RenderCopy(ren, settings, NULL, &dstButton);
				break;
			case 2:
				SDL_RenderCopy(ren, exit, NULL, &dstButton);
				break;
			default:
				break;
			}
		}

//отрисовка персонажа на блоке
		animTime += dt;//анимация стояния
		if (animTime > frameTime)
		{
			if (a < 3)
			{
				if (n < 7)
				{
					clip = { n * 110, a * 80, 110, 80 };
					n++;
				}
				else
				{
					a++;
					n = 0;
				}
			}
			else
				a = 0;
			animTime -= frameTime;
		}

		SDL_RenderCopy(ren, persStand, &clip, &dstRectPers);
		SDL_RenderCopy(ren, brick, NULL, &dstRectBrick);

		SDL_RenderPresent(ren);
	}
	exit_menu(txt, button, start, settings, exit, persStand, brick, name);
	return 3;
}