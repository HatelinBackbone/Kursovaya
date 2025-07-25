#include "Functions.h"
#include "Menu.h"

struct but//структура для хранения надписей
{
	char note[64];
	char lvl1[4] = "1";
	char l1n[128] = "\0";
	char lvl2[4] = "2";
	char l2n[128] = "\0";
	char lvl3[4] = "3";
	char l3n[128] = "\0";
};

int but_rec(const char file[], but* menu)//чтение из файла в структуру надписей
{
	FILE* f;
	if (fopen_s(&f, file, "r+") != 0)
	{
		printf_s("Не удалось открыть файл\n");
		return 1;
	}

	char buffer[128];
	char* token = NULL;
	char* next_token = NULL;

	fgets(buffer, 64, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->note, 64, token);
	buffer[0] = '\0';

	fgets(buffer, 128, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->l1n, 128, token);
	buffer[0] = '\0';

	fgets(buffer, 128, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->l2n, 128, token);
	buffer[0] = '\0';

	fgets(buffer, 128, f);
	token = strtok_s(buffer, "\n", &next_token);
	strcpy_s(menu->l3n, 128, token);
	buffer[0] = '\0';

	fclose(f);
	return 0;
}

void exit_sel(Mix_Chunk* click, TTF_Font* txt, SDL_Texture* button, SDL_Texture* b1, SDL_Texture* l1n, SDL_Texture* b2, SDL_Texture* l2n, SDL_Texture* b3, SDL_Texture* l3n, SDL_Texture* back, SDL_Texture* note)//завершение и очистка памяти
{
	Mix_FreeChunk(click);
	TTF_CloseFont(txt);
	SDL_DestroyTexture(button);
	SDL_DestroyTexture(b1);
	SDL_DestroyTexture(l1n);
	SDL_DestroyTexture(b2);
	SDL_DestroyTexture(l2n);
	SDL_DestroyTexture(b3);
	SDL_DestroyTexture(l3n);
	SDL_DestroyTexture(back);
	SDL_DestroyTexture(note);
}

int lvl_sel(SDL_Renderer* ren, bool sel)//меню выбора уровней	
{
	but but;
	SDL_Event ev;
	SDL_Rect dstNote = { 300,50,700,150 }, dstLNote = { 100,500,700,100 }, srcButton = { 0,0,0,0 }, srcBack = { 0,0,0,0 }, dstBack = {10,10,24,23}, dstButton = { 0,0,50,50 };
	SDL_Texture* button = NULL, * b1 = NULL, * b2 = NULL, * b3 = NULL, * note = NULL, * back = NULL, *l1n = NULL, * l2n = NULL, * l3n = NULL;
	TTF_Font* txt = NULL;
	SDL_Color txtc = { 0, 0, 0, 0 }; // цвет текста

	Mix_Chunk* click = NULL;
	click = Mix_LoadWAV("Click.wav");

	int n = 0;
	int xm = 0;
	int ym = 0;

	bool n1 = false, n2 = false, n3 = false;

	button = load("But_Kv.png", &srcButton, ren, button);//кнопки в меню
	back = load("Back.png", &srcBack, ren, back);

	txt = TTF_OpenFont("font.ttf", 100);//открытие шрифта для меню

	but_rec("Select.txt", &but);//запись данных из файла в структуру
	note = create_text_texture(ren, but.note, txt, txtc, &dstLNote);//создание текстовых текстур
	b1 = create_text_texture(ren, but.lvl1, txt, txtc, &dstButton);
	l1n = create_text_texture(ren, but.l1n, txt, txtc, &dstLNote);
	b2 = create_text_texture(ren, but.lvl2, txt, txtc, &dstButton);
	l2n = create_text_texture(ren, but.l2n, txt, txtc, &dstLNote);
	b3 = create_text_texture(ren, but.lvl3, txt, txtc, &dstButton);
	l3n = create_text_texture(ren, but.l3n, txt, txtc, &dstLNote);

	while (sel)
	{
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); //фон 
		SDL_RenderClear(ren);

		while (SDL_PollEvent(&ev))//обработка событий
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				exit_sel(click, txt, button, b1, l1n, b2, l2n, b3, l3n, back, note);
				return 0;
				break;
			case SDL_MOUSEMOTION://если мышь наведена на кнопки уровней
				xm = ev.motion.x;
				ym = ev.motion.y;
				if (ym >= 300 && ym <= 400)
				{
					n1 = false;
					n2 = false;
					n3 = false;
					if (xm >= 490 && xm <= 580)
						n1 = true;
					if (xm >= 600 && xm <= 690)
						n2 = true;
					if (xm >= 710 && xm <= 800)
						n3 = true;
				}
				else
				{
					n1 = false;
					n2 = false;
					n3 = false;
				}
				break;
			case SDL_MOUSEBUTTONDOWN://клик на кнопку уровня
				switch (ev.button.button)
				{
				case SDL_BUTTON_LEFT:
					int x = ev.button.x;
					int y = ev.button.y;
					if (y >= 10 && y <= 34)//выход в главное меню
						if (x >= 10 && x <= 33)
						{
							Mix_PlayChannel(-1, click, 0);
							exit_sel(click, txt, button, b1, l1n, b2, l2n, b3, l3n, back, note);
							return -1;
						}
					if (y >= 300 && y <= 400)//клики по кнопкам выбора уровней
					{
						if (x >= 490 && x <= 580)//1 lvl
						{
							exit_sel(click, txt, button, b1, l1n, b2, l2n, b3, l3n, back, note);
							return 1;
						}
						if (xm >= 600 && xm <= 690)
						{
							exit_sel(click, txt, button, b1, l1n, b2, l2n, b3, l3n, back, note);
							return 2;
						}
						if (xm >= 710 && xm <= 800)
						{
							exit_sel(click, txt, button, b1, l1n, b2, l2n, b3, l3n, back, note);
							return 3;
						}
					}
				}
			}
		}

		if(n1)//отрисовка подписей к уровням
			SDL_RenderCopy(ren, l1n, NULL, &dstLNote);
		if(n2)
			SDL_RenderCopy(ren, l2n, NULL, &dstLNote);
		if (n3)
			SDL_RenderCopy(ren, l3n, NULL, &dstLNote);

		SDL_RenderCopy(ren, note, NULL, &dstNote);//запись над уровнями
		SDL_RenderCopy(ren, back, NULL, &dstBack);//стрелка выхода в меню

		for (n=0; n < 3; n++)//отрисовка кнопок уровней с надписями на них
		{
			dstButton = { 490 + n * 110,300,90,100 };
			SDL_RenderCopy(ren, button, NULL, &dstButton);
			switch (n)
			{
			case 0:
				SDL_RenderCopy(ren, b1, NULL, &dstButton);
				break;
			case 1:
				SDL_RenderCopy(ren, b2, NULL, &dstButton);
				break;
			case 2:
				SDL_RenderCopy(ren, b3, NULL, &dstButton);
				break;
			}
		}

		SDL_RenderPresent(ren);
	}
}