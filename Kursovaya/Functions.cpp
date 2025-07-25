#include"Functions.h"

const int brick_w = 72;
const int brick_h = 72;

int init() //инициализация библиотек
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		const char* s = SDL_GetError();
		printf_s("Ошибка инициализации SDL\n");
		return 1;
	}
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		const char* s = IMG_GetError();
		printf_s("Ошибка инициализации IMG-библиотеки %s\n", s);
		return 1;
	}
	if (TTF_Init() != 0)
	{
		const char* s = TTF_GetError();
		printf_s("Ошибка инициализации TTF %s\n", s);
		return 1;
	}
	/*if ((Mix_Init(MIX_INIT_OGG)) != MIX_INIT_OGG)
	{
		const char* s = Mix_GetError();
		printf_s("Ошибка инициализации MIX-библиотеки %s\n", s);
		return 1;
	}*/
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) 
	{
		const char* s = Mix_GetError();
		printf_s("Ошибка инициализации MIX-библиотеки %s\n", s);
		return 1;
	}
	
	return 0;
}

SDL_Texture* load(const char* file, SDL_Rect* rect, SDL_Renderer* ren, SDL_Texture* tex) //создание текстур
{
	SDL_Surface* surf = NULL;
	surf = IMG_Load(file);
	if (surf == NULL)
	{
		const char* s = IMG_GetError();
		printf_s("Ошибка зарузки изображения %s: %s\n", file, s);
		return NULL;
	}
	rect->x = 0;
	rect->y = 0;
	rect->w = surf->w;
	rect->h = surf->h;
	tex = SDL_CreateTextureFromSurface(ren, surf);
	if (tex == NULL)
	{
		const char* s = SDL_GetError();
		printf_s("Ошибка создания текстуры персонажа %s\n", s);
		return NULL;
	}

	SDL_FreeSurface(surf);
	return tex;
}

int level_info(const char file[], map* Map) //чтение уровня из файла
{
	FILE* f;
	if (fopen_s(&f, file, "r+") != 0)
	{
		printf_s("Не удалось открыть файл\n");
		return 1;
	}

	fgets(Map->lvl, 16, f);
	fgets(Map->note, 256, f);
	fscanf_s(f, "%d %d\n", &Map->w, &Map->h);
	printf_s("%d %d\n", Map->w, Map->h);

	Map->rect = (char**)malloc(sizeof(char*) * Map->h); //выделение памяти под карту
	char buffer[256];
	for (int i = 0; i < Map->h; i++)
	{
		fgets(buffer, 256, f);
		int n = (int)strlen(buffer)+1;
		Map->rect[i] = (char*)malloc(sizeof(char) * n);
		strcpy_s(Map->rect[i], n, buffer);
		buffer[0] = '\0';
		printf_s("%s", Map->rect[i]);
	}
	printf_s("\n");

	fclose(f);
	return 0;
}

int maprender(map* map, SDL_Renderer* ren, SDL_Texture* brick, SDL_Texture* doorc, SDL_Texture* dooro, SDL_Rect* dstRectPers) // отрисовка карты
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
					rd = { x-28, y-28, 100, 100 };
					break;
				}
				if (i == map->h - 1 && j == map->w - 1)
				{
					if (dstRectPers->x + 100 >= rd.x && dstRectPers->x <= rd.x + 100 && dstRectPers->y + 80 > rd.y)
						SDL_RenderCopy(ren, dooro, NULL, &rd);
					else
						SDL_RenderCopy(ren, doorc, NULL, &rd);
					mapren = false;
				}
			}
	}
	return 0;
}

SDL_Texture* create_text_texture(SDL_Renderer* ren, const char* text, TTF_Font* font, SDL_Color color, SDL_Rect* rect) //создание текстуры текста
{
	SDL_Surface* surf = NULL;
	surf = TTF_RenderUTF8_Blended(font, text, color);
	rect->w = surf->w;
	rect->h = surf->h;
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);
	return texture;
}

SDL_Texture* timer(Uint16 time_s, Uint16 time_m, SDL_Texture* time, SDL_Renderer* ren, TTF_Font* text, SDL_Color tc, SDL_Rect* time_r)//таймер
{
	char ch_1[8];
	char ch_2[8];

	SDL_DestroyTexture(time);//уничтожение старой текстуры таймера

	sprintf(ch_1, "%d ", time_m);//создание текстуры таймера
	sprintf(ch_2, ": %d", time_s);
	strcat(ch_1, ch_2);
	time = create_text_texture(ren, ch_1, text, tc, time_r);
	ch_1[0] = '\0';
	ch_2[0] = '\0';

	return time;
}

int Quit(SDL_Renderer* ren, SDL_Window* win)
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	Mix_CloseAudio();
	return 0;
}