#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *banner = ""
"  ______ _                                \n"
" / _____) |                               \n"
"( (____ | | _____ _____ ____  _____  ____ \n"
" \\____ \\| || ___ | ___ |  _ \\| ___ |/ ___)\n"
" _____) ) || ____| ____| |_| | ____| |    \n"
"(______/ \\_)_____)_____)  __/|_____)_|    \n"
"                       |_|   \n";

void init_proc()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

typedef struct _Timer {
    char buf[8];
    unsigned int times;
    unsigned int moretimes;
} Timer;

int main()
{
    Timer t = {
        .times = 0,
        .moretimes = 8*60*60,
    };
    init_proc();

    puts(banner);
    puts("** Sleeper makes you more love sleeping ! **");
    sleep(1);
    puts("First, you need to sleep 10 seconds.");

    for (int i = 0; i < 9; i++) {
        switch (i % 3)
        {
        case 0:
            printf("\rz  ");
            break;
        case 1:
            printf("\rzz ");
            break;
        case 2:
            printf("\rzzZ");
            break;
        }
        sleep(1);
    }
    puts("\nGreat !");
    puts("Now, talk me how much sleep do you need more ?");

    fgets(t.buf, 0x10, stdin);
    t.times = atoi(t.buf);
    sleep(t.times);

    puts("More, more... ( ˘ ω˘)zzZ");
    sleep(t.moretimes);

    puts("Wonderful! It's time to do some work !");
    system("sh");

    return 0;
}
