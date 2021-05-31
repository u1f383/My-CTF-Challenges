#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *banner = ""
" ________   ________  ________  _______   ________     \n"
"|\\   ___  \\|\\   __  \\|\\   __  \\|\\  ___ \\ |\\   __  \\    \n"
"\\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\   __/|\\ \\  \\|\\  \\   \n"
" \\ \\  \\\\ \\  \\ \\  \\\\\\  \\ \\   ____\\ \\  \\_|/_\\ \\   _  _\\  \n"
"  \\ \\  \\\\ \\  \\ \\  \\\\\\  \\ \\  \\___|\\ \\  \\_|\\ \\ \\  \\\\  \\| \n"
"   \\ \\__\\\\ \\__\\ \\_______\\ \\__\\    \\ \\_______\\ \\__\\\\ _\\ \n"
"    \\|__| \\|__|\\|_______|\\|__|     \\|_______|\\|__|\\|__|\n";
                                                       

void init_proc()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int main()
{
    char code[0x40] = {0};
    int nops[10];

    for (int i = 0; i < 10; i++) {
        nops[i] = rand() % 0x40;
    }

    init_proc();
    puts(banner);
    puts("Noper is tool can replace your code with nop randomly.\n\n");
    puts("Give me some code:");
    read(0, code, 0x40);
    
    for (int i = 0; i < 10; i++) {
        code[ nops[i] ] = '\x90';
    }

    (*(void(*)()) code)();

    return 0;
}