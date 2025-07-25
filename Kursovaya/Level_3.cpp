#include"Functions.h"

const int brick_w = 72;
const int brick_h = 72;

int Quit_3(SDL_Texture* persStand, SDL_Texture* persRun, SDL_Texture* brick, SDL_Texture* dooro, SDL_Texture* doorc, SDL_Texture* text_tex, SDL_Texture* pause,
	map* map, TTF_Font* text, SDL_Texture* time, Mix_Chunk* steps,Mix_Chunk* swap,Mix_Chunk* click, Mix_Chunk* door)
{
	Mix_FreeChunk(door);
	Mix_FreeChunk(steps);
	Mix_FreeChunk(swap);
	Mix_FreeChunk(click);
	SDL_DestroyTexture(persStand);
	SDL_DestroyTexture(persRun);
	SDL_DestroyTexture(brick);
	SDL_DestroyTexture(dooro);
	SDL_DestroyTexture(doorc);
	SDL_DestroyTexture(text_tex);
	SDL_DestroyTexture(pause);
	SDL_DestroyTexture(time);
	TTF_CloseFont(text);
	for (int k = 0; k < map->h; k++)
		free(map->rect[k]);
	free(map->rect);
	return 0;
}

int maprender_3(map* map, SDL_Renderer* ren, SDL_Texture* brick, SDL_Texture* doorc, SDL_Texture* dooro, SDL_Rect* dstRectPers, bool open) // отрисовка карты и завершение уровн€
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
					if (open)
					{
						SDL_RenderCopy(ren, dooro, NULL, &rd);
					}
					else
						SDL_RenderCopy(ren, doorc, NULL, &rd);
					if (dstRectPers->x + dstRectPers->w -20 >= rd.x && dstRectPers->y + dstRectPers->h >= rd.y && open)
						return 1;

					mapren = false;
				}
			}
	}
	return 0;
}

int lvl_3(SDL_Renderer* ren, bool run_3)
{
	SDL_Rect clip1 = { 0, 0, 110, 80 }, clip2 = { 0, 0, 110, 80 }, srcRectPers = { 0,0,0,0 }, dstRectPers = { 0,0,0,0 }, srcRectBrick = { 0,0,0,0 },
		srcRectDooro = { 0,0,0,0 }, srcRectDoorc = { 0,0,0,0 }, rect_t = { 360, 72, 0, 0 }, brRect1 = { 576,432,144,72 }, brRect2 = {288,288,72,72}, brRect3 = {936,288,72,72},
		brRect4 = { 72,216,144,72 }, brRect5 = { 1080,216,144,72 }, brRectUP = {0,0,1296,72}, colPers = { 0,0,0,0 }, srcPause = { 0,0,0,0 }, dstPause = { 0,0,0,0 };
	SDL_Rect floorRect = { 0, 648, 1280, 72 }, time_r = { 1100, 0, 0, 0 };
	Vector2f posPers = { 100, 400 };
	Vector2f old_posPers = { 100, 400 };
	Vector2f speedPers = { 0, 0 };
	Vector2f globalAccel = { 0, 0 };

	TTF_Font* text; //шрифт текста на уровн€х
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
	SDL_Texture* pause = NULL;
	SDL_Texture* time = NULL;

	Mix_Chunk* click = NULL;
	Mix_Chunk* steps = NULL;
	Mix_Chunk* swap = NULL;
	Mix_Chunk* door = NULL;

	click = Mix_LoadWAV("Click.wav");
	steps = Mix_LoadWAV("Steps.wav");
	swap = Mix_LoadWAV("Swap.wav");
	door = Mix_LoadWAV("Door_open.wav");

	bool makeJump = false;
	bool onfloor = false;
	bool isIdle = true;
	bool isRunning = true;
	bool fall = true;
	bool open = false;
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
	int mx = 0, my = 0;
	int dx = 0, dy = 0;
	int n = 0, k = 0, a = 0;
	int ot = SDL_GetTicks(), nt;
	int DeathCounter = 0;
	int c = 1;

	char* tok = NULL;
	char* next_tok = NULL;

	map map = { NULL, NULL, 0, 0, NULL };

	text = TTF_OpenFont("font.ttf", 90);//открытие шрифта

	persStand = load("Sprites/StandSprites.png", &srcRectPers, ren, persStand);
	dstRectPers = { 0 ,0, 110, 80 };

	persRun = load("Sprites/RunSprites.png", &srcRectPers, ren, persRun);

	brick = load("KenneY/PNG/brick.png", &srcRectBrick, ren, brick);

	dooro = load("KenneY/PNG/DoorO.png", &srcRectDooro, ren, dooro);

	doorc = load("KenneY/PNG/DoorC.png", &srcRectDoorc, ren, doorc);

	pause = load("pause.png", &srcPause, ren, pause);
	dstPause = { 1152,72,72,72 };

	if (level_info("Level3.txt", &map) == 1)//считывание в структуру 1 уровн€
		return 1;

	tok = strtok_s(map.note, "\n", &next_tok);	//текст дл€ уровн€ 1
	text_tex = create_text_texture(ren, tok, text, tc, &rect_t);

	while (run_3)
	{
		nt = SDL_GetTicks();
		dt = (nt - ot) / 1000.0f;
		ot = nt;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				run_3 = false;
				break;
			case SDL_WINDOWEVENT:
				ot = SDL_GetTicks();
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					run_3 = false;
					break;
				case SDL_QUIT:
					run_3 = false;
					break;
				case SDLK_SPACE:
					makeJump = true;
					break;
				}
				case SDL_MOUSEBUTTONDOWN://попадание в паузу
					if (c == 3)
					{
							switch (e.button.button)
							{
							case SDL_BUTTON_LEFT:
								x = e.button.x;
								y = e.button.y;
								if (x >= dstPause.x && x <= dstPause.x + dstPause.w)
									if (y >= dstPause.y && y <= dstPause.y + dstPause.h)
									{
										dstPause = { 0,0,0,0 };
										Mix_PlayChannel(2, door, 0);
										open = true;
									}
								break;
							}
					}
				}
			}
			//механика уровн€
			speedPers.x += globalAccel.x * dt;
			speedPers.y += globalAccel.y * dt;

			posPers.x += speedPers.x * dt;
			posPers.y += speedPers.y * dt;

			if (posPers.y + dstRectPers.h > floorRect.y && posPers.y + dstRectPers.h < floorRect.y + floorRect.h)//коллизи€ с полом
			{
				posPers.y = floorRect.y - dstRectPers.h;
				speedPers.y = 0;
				onfloor = true;
				fall = false;
			}

			if (posPers.x <= -30 || posPers.x + dstRectPers.w >= 1316 || posPers.y >= 720)//выход за пределы экрана
			{
				posPers = { 100,550 };
				DeathCounter++;
			}

			if (posPers.y <= 495 && posPers.y >= 490)//коллизи€ с боковинами
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

			if ((colPers.y < brRect1.y + brRect1.h && colPers.y >= brRect1.y + brRect1.h - 1) && (colPers.x + colPers.w > brRect1.x && colPers.x < brRect1.x + brRect1.w))//коллизи€ с низом блока 1
			{
				speedPers.y = 0;
				posPers.y = brRect1.y + brRect1.h;

			}
			else
				if ((colPers.y > brRect1.y || colPers.y < brRect1.y + brRect1.h) && (colPers.y + colPers.h > brRect1.y) && (colPers.y < brRect1.y + brRect1.h) && (colPers.x + colPers.w > brRect1.x && colPers.x < brRect1.x + 2))//коллизи€ с левым боком блока1
				{
					speedPers.x = 0;
					posPers.x = brRect1.x - colPers.w - 20;
				}
				else
					if ((colPers.y > brRect1.y || colPers.y < brRect1.y + brRect1.h) && (colPers.y + colPers.h > brRect1.y) && (colPers.y < brRect1.y + brRect1.h) && (colPers.x < brRect1.x + brRect1.w && colPers.x > brRect1.x + (brRect1.w - 2)))//коллизи€ с правым боком блока1
					{
						speedPers.x = 0;
						posPers.x = brRect1.x + brRect1.w - 20;
					}
					else
						if ((posPers.y + dstRectPers.h > brRect1.y && posPers.y + dstRectPers.h <= brRect1.y + 1) && (colPers.x + colPers.w > brRect1.x && colPers.x < brRect1.x + brRect1.w))//коллизи€ с верхом блока1
						{
							posPers.y = brRect1.y - dstRectPers.h;
							speedPers.y = 0;
							onfloor = true;
							fall = false;
						}
			if ((colPers.y < brRect2.y + brRect2.h && colPers.y >= brRect2.y + brRect2.h - 1) && (colPers.x + colPers.w > brRect2.x && colPers.x < brRect2.x + brRect2.w))//коллизи€ с низом блока2
			{
				speedPers.y = 0;
				posPers.y = brRect2.y + brRect2.h;

			}
			else
				if ((colPers.y > brRect2.y || colPers.y < brRect2.y + brRect2.h) && (colPers.y + colPers.h > brRect2.y) && (colPers.y < brRect2.y + brRect2.h) && (colPers.x + colPers.w > brRect2.x && colPers.x < brRect2.x + 2))//коллизи€ с левым боком блока2
				{
					speedPers.x = 0;
					posPers.x = brRect2.x - colPers.w - 20;
				}
				else
					if ((colPers.y > brRect2.y || colPers.y < brRect2.y + brRect2.h) && (colPers.y + colPers.h > brRect2.y) && (colPers.y < brRect2.y + brRect2.h) && (colPers.x < brRect2.x + brRect2.w && colPers.x > brRect2.x + (brRect2.w - 2)))//коллизи€ с правым боком блока2
					{
						speedPers.x = 0;
						posPers.x = brRect2.x + brRect2.w - 20;
					}
					else
						if ((posPers.y + dstRectPers.h > brRect2.y && posPers.y + dstRectPers.h <= brRect2.y + 1) && (colPers.x + colPers.w > brRect2.x && colPers.x < brRect2.x + brRect2.w))//коллизи€ с верхом блока2
						{
							posPers.y = brRect2.y - dstRectPers.h;
							speedPers.y = 0;
							onfloor = true;
							fall = false;
						}
			if ((colPers.y < brRect3.y + brRect3.h && colPers.y >= brRect3.y + brRect3.h - 1) && (colPers.x + colPers.w > brRect3.x && colPers.x < brRect3.x + brRect3.w))//коллизи€ с низом блока3
			{
				speedPers.y = 0;
				posPers.y = brRect3.y + brRect3.h;

			}
			else
				if ((colPers.y > brRect3.y || colPers.y < brRect3.y + brRect3.h) && (colPers.y + colPers.h > brRect3.y) && (colPers.y < brRect3.y + brRect3.h) && (colPers.x + colPers.w > brRect3.x && colPers.x < brRect3.x + 2))//коллизи€ с левым боком блока3
				{
					speedPers.x = 0;
					posPers.x = brRect3.x - colPers.w - 20;
				}
				else
					if ((colPers.y > brRect3.y || colPers.y < brRect3.y + brRect3.h) && (colPers.y + colPers.h > brRect3.y) && (colPers.y < brRect3.y + brRect3.h) && (colPers.x < brRect3.x + brRect3.w && colPers.x > brRect3.x + (brRect3.w - 2)))//коллизи€ с правым боком блока3
					{
						speedPers.x = 0;
						posPers.x = brRect3.x + brRect3.w - 20;
					}
					else
						if ((posPers.y + dstRectPers.h > brRect3.y && posPers.y + dstRectPers.h <= brRect3.y + 1) && (colPers.x + colPers.w > brRect3.x && colPers.x < brRect3.x + brRect3.w))//коллизи€ с верхом блока3
						{
							posPers.y = brRect3.y - dstRectPers.h;
							speedPers.y = 0;
							onfloor = true;
							fall = false;
						}
			if ((colPers.y < brRect4.y + brRect4.h && colPers.y >= brRect4.y + brRect4.h - 1) && (colPers.x + colPers.w > brRect4.x && colPers.x < brRect4.x + brRect4.w))//коллизи€ с низом блока4
			{
				speedPers.y = 0;
				posPers.y = brRect4.y + brRect4.h;

			}
			else
				if ((colPers.y > brRect4.y || colPers.y < brRect4.y + brRect4.h) && (colPers.y + colPers.h > brRect4.y) && (colPers.y < brRect4.y + brRect4.h) && (colPers.x + colPers.w > brRect4.x && colPers.x < brRect4.x + 2))//коллизи€ с левым боком блока4
				{
					speedPers.x = 0;
					posPers.x = brRect4.x - colPers.w - 20;
				}
				else
					if ((colPers.y > brRect4.y || colPers.y < brRect4.y + brRect4.h) && (colPers.y + colPers.h > brRect4.y) && (colPers.y < brRect4.y + brRect4.h) && (colPers.x < brRect4.x + brRect4.w && colPers.x > brRect4.x + (brRect4.w - 2)))//коллизи€ с правым боком блока4
					{
						speedPers.x = 0;
						posPers.x = brRect4.x + brRect4.w - 20;
					}
					else
						if ((posPers.y + dstRectPers.h > brRect4.y && posPers.y + dstRectPers.h <= brRect4.y + 1) && (colPers.x + colPers.w > brRect4.x && colPers.x < brRect4.x + brRect4.w))//коллизи€ с верхом блока4
						{
							posPers.y = brRect4.y - dstRectPers.h;
							speedPers.y = 0;
							onfloor = true;
							fall = false;
						}
			if ((colPers.y < brRect5.y + brRect5.h && colPers.y >= brRect5.y + brRect5.h - 1) && (colPers.x + colPers.w > brRect5.x && colPers.x < brRect5.x + brRect5.w))//коллизи€ с низом блока5
			{
				speedPers.y = 0;
				posPers.y = brRect5.y + brRect5.h;

			}
			else
				if ((colPers.y > brRect5.y || colPers.y < brRect5.y + brRect5.h) && (colPers.y + colPers.h > brRect5.y) && (colPers.y < brRect5.y + brRect5.h) && (colPers.x + colPers.w > brRect5.x && colPers.x < brRect5.x + 2))//коллизи€ с левым боком блока5
				{
					speedPers.x = 0;
					posPers.x = brRect5.x - colPers.w - 20;
				}
				else
					if ((colPers.y > brRect5.y || colPers.y < brRect5.y + brRect5.h) && (colPers.y + colPers.h > brRect5.y) && (colPers.y < brRect5.y + brRect5.h) && (colPers.x < brRect5.x + brRect5.w && colPers.x > brRect5.x + (brRect5.w - 2)))//коллизи€ с правым боком блока5
					{
						speedPers.x = 0;
						posPers.x = brRect5.x + brRect5.w - 20;
					}
					else
						if ((posPers.y + dstRectPers.h > brRect5.y && posPers.y + dstRectPers.h <= brRect5.y + 1) && (colPers.x + colPers.w > brRect5.x && colPers.x < brRect5.x + brRect5.w))//коллизи€ с верхом блока5
						{
							posPers.y = brRect5.y - dstRectPers.h;
							speedPers.y = 0;
							onfloor = true;
							fall = false;
						}

			if ((colPers.y < brRectUP.y + brRectUP.h && colPers.y >= brRectUP.y + brRectUP.h - 1) && (colPers.x + colPers.w > brRectUP.x && colPers.x < brRectUP.x + brRectUP.w))//коллизи€ с потолком
			{
				speedPers.y = 0;
				posPers.y = brRectUP.y + brRectUP.h;

			}

			switch (c)//коллизии с паузой	
			{
			case 1:
				if ((colPers.y < dstPause.y + dstPause.h && colPers.y >= dstPause.y + dstPause.h - 1) && (colPers.x + colPers.w > dstPause.x && colPers.x < dstPause.x + dstPause.w))//коллизи€ с низом паузы справа
				{
					Mix_PlayChannel(1, swap, 0);
					dstPause = { 72,72,72,72 };
					c = 2;
				}
				else
					if ((colPers.y > brRect5.y || colPers.y < brRect5.y + brRect5.h) && (colPers.y + colPers.h > brRect5.y) && (colPers.y < brRect5.y + brRect5.h) && (colPers.x + colPers.w > brRect5.x && colPers.x < brRect5.x + 2))//коллизи€ с левым боком паузы справа
					{
						Mix_PlayChannel(1, swap, 0);
						dstPause = { 72,72,72,72 };
						c = 2;
					}
				break;
			case 2:
				if ((colPers.y < dstPause.y + dstPause.h && colPers.y >= dstPause.y + dstPause.h - 1) && (colPers.x + colPers.w > dstPause.x && colPers.x < dstPause.x + dstPause.w))//коллизи€ с низом паузы слева
				{
					Mix_PlayChannel(1, swap, 0);
					dstPause = { 288,0,720,720 };
					c = 3;
				}
				else
					if ((colPers.y > dstPause.y || colPers.y < dstPause.y + dstPause.h) && (colPers.y + colPers.h > dstPause.y) && (colPers.y < dstPause.y + dstPause.h) && (colPers.x < dstPause.x + dstPause.w && colPers.x > dstPause.x + (dstPause.w - 2)))//коллизи€ с правым боком паузы слева
					{
						Mix_PlayChannel(1, swap, 0);
						dstPause = { 288,0,720,720 };
						c = 3;
					}
				break;
			}

			if (old_posPers.y < posPers.y && !fall)//предотвращение возможности прыжка после схождени€ с платформы
				fall = true;

			if (makeJump)//прыжок
			{
				if (onfloor && !fall)
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

			dstRectPers.x = posPers.x;
			dstRectPers.y = posPers.y;
			colPers = { dstRectPers.x + 20, dstRectPers.y + 9, dstRectPers.w - 40, dstRectPers.h - 9 };//новый квадрат дл€ коллизии персонажа
			old_posPers = posPers;

			if (isIdle && onfloor) //анимаци€ сто€ни€
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

			if (!isIdle && onfloor) //анимаци€ бега
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

			//====================================================== ќ“–»—ќ¬ ј ===============================================================================
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);//фон
			SDL_RenderClear(ren);

			if (maprender_3(&map, ren, brick, doorc, dooro, &dstRectPers, open) == 1)//отрисовка карты
				passed = true;

			if (isIdle && onfloor)//отрисовка сто€щего персонажа
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

			if (globalAccel.y == 0)//дл€ корректного первого запуска
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
				time = timer(time_s, time_m, time, ren, text, tc, &time_r);
				start = SDL_GetTicks();
			}
			SDL_RenderCopy(ren, time, NULL, &time_r);//отрисовка таймера

			SDL_RenderCopy(ren, pause, NULL, &dstPause);

			SDL_RenderPresent(ren);	

			if (passed)
			{
				Quit_3(persStand, persRun, brick, dooro, doorc, text_tex, pause, &map, text, time,steps,swap,click,door);
				return 0;
			}

	}
	
	Quit_3(persStand, persRun, brick, dooro, doorc, text_tex, pause, &map, text, time,steps, swap, click,door);
	return 0;
}