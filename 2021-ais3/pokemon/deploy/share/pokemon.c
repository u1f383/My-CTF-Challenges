#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include "util.h"
#include "pokedex.h"
#include "pokemon.h"
#define inMapRange(x, y, width, height) (x >= 1 && x <= width-1) && (y >= 1 && y <= height-1)

void initProc()
{
    // for non buffer
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    // for rand seed
    srand(time(NULL));
}

void showBannar()
{
    char *tok;
    char *cursor = strdup(banner);

    cls();
    tok = strtok(cursor, "\n");
    while (tok != NULL) {
        usleep(20*1000);
        printf("%s\n", tok);
        tok = strtok(NULL, "\n");
    }
    free(cursor);
}

// http://www.csie.ntnu.edu.tw/~ghhwang/course_slices/OS/Curses_Usage.txt
void initCurses()
{
    cls();
    initscr(); // change mode to curses
    cbreak(); // input read immediately
    nonl(); // return => newline
    noecho(); // input not appear at terminal
    intrflush(stdscr, FALSE); // std screen
    keypad(stdscr, TRUE); // input map to curses.h define macro
    refresh(); // refresh/update screen
    atexit(gameEpilogue);
}

void gameEpilogue()
{
    sleep(1);
    endwin();
    cls();
}

void pressAnime(WINDOW *win)
{
    for (int i = 0; i < BtnHeight; i++)
        for (int j = 0; j < BtnWidth; j++)
            mvwaddstr(win, i, j, "@");
    
    wrefresh(win);
    usleep(50*1000);

    refreshBtnWin(win);
}

void collAnime(WINDOW *win)
{
    int lbx = 1;
    int ubx = GameWinWidth-2;
    int lby = 1;
    int uby = GameWinHeight-2;
    int i = lbx;
    int j = lby;
    int count = 0;
    int total = ubx*uby;

    while (1) {
        for (; i <= ubx; i++) {
            mvwaddstr(win, j, i, "@");
            wrefresh(win);
            beep();
            usleep(3*1000);
            count++;
        }
        i--; lby++;
        if (count >= total) break;

        for (; j <= uby; j++) {
            mvwaddstr(win, j, i, "@");
            wrefresh(win);
            beep();
            usleep(3*1000);
            count++;
        }
        j--; ubx--;
        if (count >= total) break;
        
        for (; i >= lbx; i--) {
            mvwaddstr(win, j, i, "@");
            wrefresh(win);
            beep();
            usleep(3*1000);
            count++;
        }
        i++; uby--;
        if (count >= total) break;
        
        for (; j >= lby; j--) {
            mvwaddstr(win, j, i, "@");
            wrefresh(win);
            beep();
            usleep(3*1000);
            count++;
        }
        j++; lbx++;
        if (count >= total) break;
    }
}

void winGameAnime(Pokemon *pokemon)
{
    char buf[0x50] = {0};
    char *marquee = buf;

    sprintf(buf, "%s is down ....", pokemon->name);
    runMarquee(buf);
    printf("\r%-*s", 40, "");
    
    strcpy(buf, "\rYou win the game !");
    runMarquee(buf);
}

void drawMonsters(WINDOW *win, MonsterLoc *monsters[6])
{
    for (int i = 0; i < 6; i++) {
        if (monsters[i] != NULL)
            mvwaddstr(win, monsters[i]->y, monsters[i]->x, "@");
    }
        
    wrefresh(win);
}

void refreshBtnWin(WINDOW *win)
{
    for (int i = 0; i < BtnHeight; i++)
        for (int j = 0; j < BtnWidth; j++) {
            if (i == 0 && j == 0)
                mvwaddstr(win, i, j, "X");
            else if (i == BtnHeight-1 && j == 0)
                mvwaddstr(win, i, j, "X");
            else if (i == 0 && j == BtnWidth-1)
                mvwaddstr(win, i, j, "X");
            else if (i == BtnHeight-1 && j == BtnWidth-1)
                mvwaddstr(win, i, j, "X");
            else if (i == 0 || i == BtnHeight-1)
                mvwaddstr(win, i, j, "-");
            else if (j == 0 || j == BtnWidth-1)
                mvwaddstr(win, i, j, "|");
            else
                mvwaddstr(win, i, j, " ");
        }
   
    wrefresh(win);
}

void cleanWin(WINDOW *win)
{
    for (int i = 1; i < GameWinHeight-1; i++)
        for (int j = 1; j < GameWinWidth-1; j++)
            mvwaddstr(win, i, j, " ");
}

void changeName(PokemonMaster *master)
{
    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);
    cls();
    if (changeNameTime) {
        printf("Give me new name: ");
        scanf("%30s", master->name);
        getchar();
        changeNameTime--;
        printf("Hello %s, your rest change name time is %d\n", master->name, changeNameTime);
    } else {
        puts("No more change name time !");
    }
    printf("PRESS ENTER TO CONTINUE ");
    getch();
}

void runMarquee(char buf[0x50])
{
    char *marquee = buf;
    for (int i = 0; i < strlen(buf); i++) {
        printf("%c", *marquee++);
        usleep(5*1000);
    }
    sleep(1);
}

void sacrifice(PokemonMaster *master)
{
    if (master->pokemonNum >= 3 && !hasSacrifice) {
        endwin();
        setvbuf(stdout, NULL, _IONBF, 0);
        cls();
        hasSacrifice = 1;

        char buf[0x50] = "Ok, I know you have a lot of pokemon....\n";
        char ch;

        runMarquee(buf);
        strcpy(buf, "So if you get me 2 pokemons, I will get you something good....(y/n) ");
        runMarquee(buf);

        if ((ch = getchar()) == 'y') {
            unsigned int c1, c2;
            strcpy(buf, "Give me 2 pocket ids of pokemons\n");
            runMarquee(buf);
            printf("1: ");
            scanf("%u", &c1);
            printf("2: ");
            scanf("%u", &c2);

            if (master->pocket[c1-1] != NULL
                && master->pocket[c2-1] != NULL
                && c1 < 8 && c2 < 8) {

                strcpy(buf, "Good...\n");
                runMarquee(buf);
                puts("");
                free(master->pocket[c1-1]);
                master->pocket[c1-1] = NULL;
                free(master->pocket[c2-1]);
                master->pocket[c2-1] = NULL;

                printf("Give me more one pocket id of pokemon to use golden finger: ");
                scanf("%u", &c1);

                if (c1 < 8 && master->pocket[c1-1] != NULL) {
                    int val;
                    printf("1. hp , 2. attack , 3. speed\nChoose one: ");
                    scanf("%u", &c1);
                    printf("Give me a value: ");
                    scanf("%d", &val);
                    switch (c1) {
                        case '1':
                            master->pocket[c1-1]->hp = val;
                            break;
                        case '2':
                            master->pocket[c1-1]->attack = val;
                            break;
                        case '3':
                            master->pocket[c1-1]->speed = val;
                            break;
                    }
                } else {
                    strcpy(buf, "Id is invalid\n");
                    runMarquee(buf);
                }
            }
        }
        strcpy(buf, "Goodbye...\n");
        runMarquee(buf);
        puts("");
        printf("PRESS ENTER TO CONTINUE ");
        getch();
    }
}

void showName(PokemonMaster *master)
{
    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);
    cls();
    printf("Hello, my name is %s !\n", master->name);
    printf("PRESS ENTER TO CONTINUE ");
    getch();
}

void pocketHandler(PokemonMaster *master)
{
    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);
    cls();
    showPocket(master);
    printf("PRESS ENTER TO CONTINUE ");
    getch();
}

void pokedexHandler(PokemonMaster *master)
{
    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);
    cls();
    showPokedex(master);
    printf("PRESS ENTER TO CONTINUE ");
    getch();
}

void backpackHandler(PokemonMaster *master)
{
    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);
    cls();
    showProps(master);
    printf("PRESS ENTER TO CONTINUE ");
    getch();
}

int collisionMons(int x, int y, PokemonMaster *master, MonsterLoc *monsters[6])
{
    for (int i = 0; i < 6; i++)
        if (monsters[i] != NULL && monsters[i]->x == x && monsters[i]->y == y)
            return i;

    return -1;
}

int monsterCount(MonsterLoc *monsters[6])
{
    int count = 0;
    for (int i = 0; i < 6; i++)
        if (monsters[i] == NULL)
            count++;
    
    return count;
}

int main()
{
    initProc();
    showBannar();

    PokemonMaster *master = newPokemonMaster();

    printf("What's your name, Pokémon Master ?\n> ");
    scanf("%30s", master->name);
    getchar();
    printf("Welcome to Pokemon world, %s! It's time to explore the wonderful world!\n", master->name);
    sleep(1);
    puts("\nOh, I forget to get you a Pokemon, here you are:");

    switch ((rand()%3) + 1) {
        case 1:
            catchPokemon(master, Bulbasaur());
            break;
        case 2:
            catchPokemon(master, Charmander());
            break;
        case 3:
            catchPokemon(master, Squirtle());
            break;
    }

    printf("Let start the game !\nPRESS ENTER TO CONTINUE ");
    getchar();

    // Game
    initCurses();

    int ch;
    int running = 1;
    int currX = 1;
    int currY = 1;
    MonsterLoc *monsters[6];

    WINDOW *gameWin = newwin(GameWinHeight, GameWinWidth, LINES/2-15, COLS/2-20); // game window
    WINDOW *btnW = newwin(BtnHeight, BtnWidth, LINES/2, COLS/2-5);
    WINDOW *btnS = newwin(BtnHeight, BtnWidth, LINES/2+5, COLS/2-5);
    WINDOW *btnA = newwin(BtnHeight, BtnWidth, LINES/2+2, COLS/2-25);
    WINDOW *btnD = newwin(BtnHeight, BtnWidth, LINES/2+2, COLS/2+15);

    for (int i = 0 ; i < 6; i++) {
        monsters[i] = generateMonster(i, rand()%11);
    }

    box(gameWin, '|', '-');
    box(btnW, '|', '-');
    box(btnS, '|', '-');
    box(btnA, '|', '-');
    box(btnD, '|', '-');
    refreshBtnWin(btnW);
    refreshBtnWin(btnS);
    refreshBtnWin(btnA);
    refreshBtnWin(btnD);
    drawMonsters(gameWin, monsters);
    wmove(gameWin, currY, currX);

    do {
        wrefresh(gameWin);
        ch = getch();

        switch(ch) {
            case 'w':
                if (inMapRange(currX, currY-1, GameWinWidth, GameWinHeight))
                    currY--;
                pressAnime(btnW);
                break;
            case 's':
                if (inMapRange(currX, currY+1, GameWinWidth, GameWinHeight))
                    currY++;
                pressAnime(btnS);
                break;
            case 'a':
                if (inMapRange(currX-1, currY, GameWinWidth, GameWinHeight))
                    currX--;
                pressAnime(btnA);
                break;
            case 'd':
                if (inMapRange(currX+1, currY, GameWinWidth, GameWinHeight))
                    currX++;
                pressAnime(btnD);
                break;
            case 'p': // pokedex
                pokedexHandler(master);
                break;
            case 'i': // pocket
                pocketHandler(master);
                break;
            case 'b': // backpack
                backpackHandler(master);
                break;
            case 'c':
                changeName(master);
                break;
            case 'n':
                showName(master);
                break;
            case 'x':
                sacrifice(master);
                break;
            case 'q':
                running = 0;
                break;
            case KEY_ESC:
                exit(1);
        }

        int idx;
        if ((idx = collisionMons(currX, currY, master, monsters)) != -1) {
            int win;

            collAnime(gameWin);
            endwin();
            setvbuf(stdout, NULL, _IONBF, 0);
            cls();
            win = fight(master, monsters[idx]->monster);

            if (win == 1) {
                monsters[idx]->monster->hp = 200;
                winGameAnime(monsters[idx]->monster);
                catchPokemon(master, monsters[idx]->monster);
                getProp(master);
                printf("PRESS ENTER TO CONTINUE ");
                getchar();
                getchar();

                free(monsters[idx]);
                monsters[idx] = NULL;

                if (monsterCount(monsters) == 6)
                    running = 0;
            } else if (win == 0) {
                printf("Game over !");
                exit(1);
            } else if (win == -1) {
                currX--;
            }

            wrefresh(gameWin);
            cleanWin(gameWin);
        }

        drawMonsters(gameWin, monsters);
        wmove(gameWin, currY, currX);
    } while (running);

    endwin();
    setvbuf(stdout, NULL, _IONBF, 0);

    cls();
    puts(congBanner);
    printf("PRESS ENTER TO CONTINUE ");
    getch();

    return 0;
}