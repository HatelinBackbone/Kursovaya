#include"Functions.h"
#define _CRT_SECURE_NO_WARNINGS

const int brick_w = 72;
const int brick_h = 72;

int Quit_1(SDL_Texture* persStand, SDL_Texture* persRun, SDL_Texture* brick, SDL_Texture* dooro, SDL_Texture* doorc,SDL_Texture * text_tex, map* map, TTF_Font* text,
	SDL_Texture* time)
{
	SDL_DestroyTexture(persStand);
	SDL_DestroyTexture(persRun);
	SDL_DestroyTexture(brick);
	SDL_DestroyTexture(dooro);
	SDL_DestroyTexture(doorc);
	SDL_DestroyTexture(text_tex);
	SDL_DestroyTexture(time);
	TTF_CloseFont(text);
	for (int k = 0; k < map->h; k++)
		free(map->rect[k]);
	free(map->rect);
	return 0;
}

int maprender_1(map* map, SDL_Renderer* ren, SDL_Texture* brick, SDL_Texture* doorc, SDL_Texture* dooro, SDL_Rect* dstRectPers) // отрисовка карты
{
	bool mapren = true;
	while (mapren)
	{
		SDL_Rect rd = { 0, 0, 100, 100 };
		for (int i = 0; i < map->h; i++)
			for (int j = 0; j < map->w; j++)
			{
				int x = j * brick_w;
				int y = i * brick_h;
				SDL_Rect r = { x,y,brick_w,brick_h };

				switch (map->rect[i][j])
				{
				case 'B':
					SDL_RenderCopy(ren, brick, NULL, &r);
					break;
				case 'X':
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					SDL_RenderFillRect(ren, &r);
					break;
				case 'D':
					rd = { x - 28, y - 28, 100, 100 };
					break;
				}
				if (i == map->h - 1 && j == map->w - 1)
				{
					if (dstRectPers->x + 100 >= rd.x && dstRectPers->x <= rd.x + 100 && dstRectPers->y + 80 > rd.y)
					{
						SDL_RenderCopy(ren, dooro, NULL, &rd);
						return 1;
					}
					else
						SDL_RenderCopy(ren, doorc, NULL, &rd);
					mapren = false;
				}
			}
	}
	return 0;
}

int lvl_1(SDL_Renderer* ren, bool run_1)
{ 
	SDL_Rect clip1 = { 0, 0, 110, 80 }, clip2 = { 0, 0, 110, 80 }, srcRectPers = { 0,0,0,0 }, dstRectPers = { 0,0,0,0 }, srcRectBrick = { 0,0,0,0 },
		srcRectDooro = { 0,0,0,0 }, srcRectDoorc = { 0,0,0,0 }, rect_t = { 75, 72, 0, 0 }, time_r = { 1100, 0, 0, 0 };
	SDL_Rect floorRect = { 0, 648, 1280, 72 };
	Vector2f posPers = { 100, 400 };
	Vector2f old_posPers = { 100, 400 };
	Vector2f speedPers = { 0, 0 };
	Vector2f globalAccel = { 0, 0 };

	TTF_Font* text; //шрифт текста на уровнях
	SDL_Color tc = { 0, 0, 0, 0 }; // цвет текста

	Uint32 start = 0;
	start = SDL_GetTicks();
	Uint16 time_m = 0, time_s = 0;

	SDL_Event e;
	SDL_Texture* persStand = NULL;
	SDL_Texture* persRun = NULL;
	SDL_Texture* brick = NULL;
	SDL_Texture* doorc = NULL;
	SDL_Texture* dooro = NULL;
	SDL_Texture* text_tex = NULL;
	SDL_Texture* time = NULL;

	Mix_Chunk* click = NULL;
	Mix_Chunk* steps = NULL;

	click = Mix_LoadWAV("Click.wav");
	steps = Mix_LoadWAV("Steps.wav");

	bool makeJump = false;
	bool onfloor = false;
	bool isIdle = true;
	bool isRunning = true;
	bool passed = false;

	float jumpSpeed = -800.0f;
	float speed = 300;
	float dt;
	float frameTime = 0.03f;
	float animTime = 0.0f;

	int dirPers = RIGHT;
	int frameNum = 0;
	int x = 0;
	int y = 0;
	int n = 0, k = 0, a = 0;
	int ot = SDL_GetTicks(), nt;
	int DeathCounter = 0;

	char* tok = NULL;
	char* next_tok = NULL;

	map map = { NULL, NULL, 0, 0, NULL };

	text = TTF_OpenFont("font.ttf", 100);//открытие шрифта

	persStand = load("Sprites/StandSprites.png", &srcRectPers, ren, persStand);
	dstRectPers = { 0 ,0, 110, 80 };

	persRun = load("Sprites/RunSprites.png", &srcRectPers, ren, persRun);

	brick = load("KenneY/PNG/brick.png", &srcRectBrick, ren, brick);

	dooro = load("KenneY/PNG/DoorO.png", &srcRectDooro, ren, dooro);

	doorc = load("KenneY/PNG/DoorC.png", &srcRectDoorc, ren, doorc);

	if (level_info("Level1.txt", &map) == 1)//считывание в структуру 1 уровня
		return 1;

	tok = strtok_s(map.note, "\n", &next_tok);	//текст для уровня 1
	text_tex = create_text_texture(ren, tok, text, tc, &rect_t);

	while (run_1)
	{
		nt = SDL_GetTicks();
		dt = (nt - ot) / 1000.0f;
		ot = nt;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				run_1 = false;
				break;
			case SDL_WINDOWEVENT:
				ot = SDL_GetTicks();
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					run_1 = false;
					break;
				case SDL_QUIT:
					run_1 = false;
					break;
				case SDLK_SPACE:
					makeJump = true;
					break;
				}
			}
		}

		speedPers.x += globalAccel.x * dt;//механика уровня
		speedPers.y += globalAccel.y * dt;

		posPers.x += speedPers.x * dt;
		posPers.y += speedPers.y * dt;

		if (posPers.y + dstRectPers.h > floorRect.y && posPers.y + dstRectPers.h < floorRect.y + floorRect.h)//коллизия с полом
		{
			posPers.y = floorRect.y - dstRectPers.h;
			speedPers.y = 0;
			onfloor = true;
		}

		if (posPers.x <= -30 || posPers.x + dstRectPers.w >= 1316)//выход за пределы экрана
		{
			posPers = { 100,550};
			DeathCounter++;
		}

		if (posPers.y <= 495 && posPers.y >= 490)//коллизия с боковинами
		{
			if ((posPers.x <= 42 && posPers.x >= -20) || (posPers.x + dstRectPers.w <= 92 && posPers.x + dstRectPers.w >= 0))//низ левой
			{
				posPers.y = 495;
				speedPers.y = 0;
			}
			if ((posPers.x >= 1204 && posPers.x <= 1276) || (posPers.x + dstRectPers.w >= 1244 && posPers.x + dstRectPers.w <= 1306))//низ правой
			{
				posPers.y = 495;
				speedPers.y = 0;
			}
		}
		if (posPers.y < 495)//бок левой
		{
			if (posPers.x <= 42)
			{
				speedPers.x = 0;
				posPers.x = 42;
			}
			if (posPers.x + dstRectPers.w >= 1244)//бок правой
			{
				speedPers.x = 0;
				posPers.x = 1134;
			}
		}

		if (makeJump)//прыжок
		{
			if (onfloor)
			{
				speedPers.y = jumpSpeed;
				onfloor = false;
			}
			makeJump = false;
		}

		const Uint8* state = SDL_GetKeyboardState(NULL); //повороты в лево/право
		if (!(state[SDL_SCANCODE_A] && state[SDL_SCANCODE_D]))
		{
			if (state[SDL_SCANCODE_A])
			{
				posPers.x -= speed * dt;
				dirPers = LEFT;
				isIdle = false;
			}
			if (state[SDL_SCANCODE_D])
			{
				posPers.x += speed * dt;
				dirPers = RIGHT;
				isIdle = false;
			}
		}
		if (!(state[SDL_SCANCODE_A] || state[SDL_SCANCODE_D]))
			isIdle = true;
 
		if (!isIdle && onfloor && (Mix_Playing(2)==0))
			Mix_PlayChannel(2, steps, 0);

		dstRectPers.x = posPers.x;
		dstRectPers.y = posPers.y;
		old_posPers = posPers;

		if (isIdle && onfloor) //анимация стояния
		{
			animTime += dt;
			if (animTime > frameTime)
			{
				if (a < 3)
				{
					if (n < 7)
					{
						clip1 = { n * 110, a * 80, 110, 80 };
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
		}

		if (!isIdle && onfloor) //анимация бега
		{
			animTime += dt;
			if (animTime > frameTime)
			{
				if (k < 13)
				{
					clip2 = { k * 110,0,110,80 };
					k++;
				}
				else
				{
					k = 0;
					clip2 = { 0,0,110,80 };
				}
				animTime -= frameTime;
			}
		}

		//====================================================== ОТРИСОВКА ===============================================================================
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);//фон
		SDL_RenderClear(ren);

		if (maprender_1(&map, ren, brick, doorc, dooro, &dstRectPers) == 1)//отрисовка карты
		{ 
			passed = true;
		}

		if (isIdle && onfloor)//отрисовка стоящего персонажа
		{
			switch (dirPers)
			{
			case RIGHT:
				SDL_RenderCopyEx(ren, persStand, &clip1, &dstRectPers, 0, NULL, SDL_FLIP_NONE);
				break;
			case LEFT: 
				SDL_RenderCopyEx(ren, persStand, &clip1, &dstRectPers, 0, NULL, SDL_FLIP_HORIZONTAL);
				break;
			}
		}

		if (!isIdle && onfloor)//отрисовка бегущего персонажа
		{
			switch (dirPers)
			{
			case RIGHT:
				SDL_RenderCopyEx(ren, persRun, &clip2, &dstRectPers, 0, NULL, SDL_FLIP_NONE);
				break;
			case LEFT:
				SDL_RenderCopyEx(ren, persRun, &clip2, &dstRectPers, 0, NULL, SDL_FLIP_HORIZONTAL);
				break;
			}
		}

		if (!onfloor)//отрисовка прыжка
		{
			switch (dirPers)
			{
			case RIGHT:
				SDL_RenderCopyEx(ren, persRun, &clip2, &dstRectPers, 0, NULL, SDL_FLIP_NONE);
				break;
			case LEFT:
				SDL_RenderCopyEx(ren, persRun, &clip2, &dstRectPers, 0, NULL, SDL_FLIP_HORIZONTAL);
				break;
			}
		}

		if (globalAccel.y == 0)//для корректного первого запуска
			globalAccel = { 0, 1200 };

		SDL_RenderCopy(ren, text_tex, NULL, &rect_t);//отрисовка текста на уровне

		if (SDL_GetTicks() - start == 1000)//таймер
		{
			time_s += 1;
			if (time_s % 60 == 0)
			{
				time_m += 1;
				time_s = 0;
			}
			time = timer(time_s,time_m,time,ren,text,tc,&time_r);
			start = SDL_GetTicks();
		}
		SDL_RenderCopy(ren, time, NULL, &time_r);//отрисовка таймера

		SDL_RenderPresent(ren);
		
		if (passed)
		{
			Quit_1(persStand, persRun, brick, dooro, doorc, text_tex, &map, text, time);
			return 1;
		}
	}

	Quit_1(persStand, persRun, brick, dooro, doorc, text_tex, &map, text, time);
	return 0;
}