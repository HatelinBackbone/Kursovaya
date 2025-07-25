#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include<SDL_mixer.h>

#define _CRT_SECURE_NO_WARNINGS
#define LEFT	1
#define RIGHT	2

struct Vector2f
{
	float x, y;
};

struct map //��������� ������ 
{
	char lvl[16];
	char note[256];
	int w, h;
	char** rect;
};

int init();//������������� ���������
SDL_Texture* load(const char* file,SDL_Rect* rect, SDL_Renderer* ren, SDL_Texture* tex);//�������� �������� �� �����
int maprender(map* map, SDL_Renderer* ren, SDL_Texture* brick, SDL_Texture* doorc, SDL_Texture* dooro, SDL_Rect* dstRectPers);//��������� ����� �� ���������
int level_info(const char file[], map *Map);//������ ������ �� ����� � ���������
int Quit(SDL_Renderer* ren, SDL_Window* win);//�������� ���� ��������� � ������������ ������
SDL_Texture* create_text_texture(SDL_Renderer* ren, const char* text, TTF_Font* font, SDL_Color color, SDL_Rect* rect);//�������� �������� ������
SDL_Texture* timer(Uint16 time_s, Uint16 time_m, SDL_Texture* time, SDL_Renderer* ren, TTF_Font* text, SDL_Color tc, SDL_Rect* time_r);//������