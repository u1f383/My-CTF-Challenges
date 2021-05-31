#pragma once
#include <curses.h>
#define cls() printf("\e[1;1H\e[2J")
#define KEY_ESC 27
#define GameWinWidth 40
#define GameWinHeight 10
#define BtnWidth 10
#define BtnHeight 3

char *note = NULL;
int changeNameTime = 3;
int hasSacrifice = 0;

void gameEpilogue();
void initProc();
void initCurses();
void showBannar();
void runMarquee(char buf[0x50]);
void cleanWin(WINDOW *win);
void pressAnime(WINDOW *win);
void collAnime(WINDOW *win);
void refreshBtnWin(WINDOW *win);
void pokedexHandler(PokemonMaster *master);
void pocketHandler(PokemonMaster *master);
void backpackHandler(PokemonMaster *master);
void showName(PokemonMaster *master);
void changeName(PokemonMaster *master);
void sacrifice(PokemonMaster *master);
int collisionMons(int x, int y, PokemonMaster *master, MonsterLoc *monsters[6]);
int inMapRange(int x, int y, int width, int height);