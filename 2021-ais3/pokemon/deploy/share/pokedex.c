#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pokedex.h"
#include "util.h"
#define TYPE_GRASS 1
#define TYPE_FIRE 2
#define TYPE_WATER 3
#define TYPE_PSYCHIC 4
#define TYPE_NORMAL 5
#define cls() printf("\e[1;1H\e[2J")

static char *TYPE[] = {"grass", "fire", "water", "psychic", "normal"};

PokemonMaster *newPokemonMaster()
{
    PokemonMaster *pkMaster = (PokemonMaster *)malloc(sizeof(PokemonMaster));

    for (int i = 0; i < 8; i++)
        pkMaster->pocket[i] = NULL;

    for (int i = 0; i < 4; i++)
        pkMaster->props[i] = NULL;

    pkMaster->props[0] = strdup("Kill Potion");
    pkMaster->props[1] = strdup("HP MAX Potion");
    memset(pkMaster->pokedex, 0, 0x10);
    memset(pkMaster->name, 0, 0x30);
    pkMaster->pokemonNum = 0;
    
    return pkMaster;
}

void catchPokemon(PokemonMaster *master, Pokemon *pokemon)
{
    for (int i = 0; i < 8; i++) {
        if (master->pocket[i] == NULL) {
            master->pocket[i] = pokemon;
            master->pokedex[ pokemon->id-1 ] = 1;
            master->pokemonNum++;
            printf("\nCongrats! Get a new pokemon \033[35m%s\033[39m", pokemon->name);
            showPokemon(master, i);

            return;
        }
    }
    puts("Sorry, your pocket is filled...");
    releasePokemon(pokemon);
}

void releasePokemon(Pokemon *pokemon)
{
    free(pokemon);
}

void showPokemon(PokemonMaster *master, unsigned int idx)
{
    if (idx < 8 && master->pocket[idx] != NULL) {
        Pokemon *pokemon = master->pocket[idx];

        puts("\n--------Pokemon Information--------");
        puts(pokemon->icon);
        printf("[name]  %s\n[id]    %u\n[hp]    %d\n[atk]   %u\n[speed] %u\n[type]  %s\n\n",
                pokemon->name, pokemon->id, pokemon->hp, pokemon->attack, pokemon->speed, TYPE[pokemon->type-1]);
    }
}

void showPocket(PokemonMaster *master)
{
    for (int i = 0; i < 8; i++)
        showPokemon(master, i);
}

void browsePocket(PokemonMaster *master)
{
    for (int i = 0; i < 8; i++) {
        if (master->pocket[i] != NULL) {
            printf("[%u] %s  ", i+1, master->pocket[i]->name);
        }
    }
    printf("\n");
}

void getProp(PokemonMaster *master)
{
    int i = 0;

    while (i < 4 && master->props[i] != NULL) i++;

    if (i == 4) {
        puts("\nYour backpack is full !");
        return;
    }
    master->props[i] = strdup("Change Name Card");
    puts("\nGet a \"Change Name Card\", it can change the name of your pokemon !");
}

void showProps(PokemonMaster *master)
{
    puts("\n--------Prop Information--------");
    for (int i = 0; i < 4; i++) {
        if (master->props[i] != NULL) {
            printf("[%d] %s\n", i+1, master->props[i]);
        } else {
            printf("[%d] empty\n", i+1);
        }
    }
}

Pokemon *findPokemon(PokemonMaster *master, unsigned int id)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (master->pocket[i] != NULL && master->pocket[i]->id == id)
            break;
    }
    
    return master->pocket[i];
}

void showPokedex(PokemonMaster *master)
{
    puts("\n--------Pokedex Information--------");
    for (int i = 0; i < 0x10; i++) {
        if (master->pokedex[i]) {
            Pokemon *pokemon = findPokemon(master, i+1);
            printf("[%u] %s, hp: %d\n", i+1, pokemon->name, pokemon->hp);
        } else {
            printf("[%u] Unknown\n", i+1);
        }
    }
}

int countBytes(char *buf, char *damage)
{
    int bytes = 0;
    int damageIdx = 0;
    int dec = 1;
    long long int curVal = atol(buf);

    while (curVal) {
        *(damage + damageIdx) = curVal & 0xff;
        curVal >>= 8;
        bytes++;
        damageIdx++;
    }

    return bytes;
}

void useProp(PokemonMaster *master, Pokemon *monster, int idx) // idx is signed => 
{
    char *prop = master->props[idx];
    
    if (prop == NULL)
        return ;
    
    if (strcmp(prop, "Change Name Card") == 0) {
        long long unsigned int pocketID;
        char ch;
        
        browsePocket(master);
        printf("Which pokemon you want to choose: ");
        scanf("%llu", &pocketID); //leak code base
        printf("Pocket id: %llu, sure ? (y/n) ", pocketID);
        getchar();
        
        if ((ch = getchar()) == 'y' && pocketID < 0x8 && master->pocket[ pocketID-1 ]) {
            char buf[0x30];
            
            getchar();
            printf("Enter a new name: ");
            scanf("%30s", buf);
            strcpy(master->pocket[ pocketID-1 ]->name, buf);
            printf("OK, your pokemon get a new name \"%s\" now\n", master->pocket[ pocketID-1 ]->name);
            printf("Use %s successfully\n", prop);
        } else {
            printf("Use %s unsuccessfully\n", prop);
            return;
        }
    } else if (strcmp(prop, "HP MAX Potion") == 0) {
        long long unsigned int pocketID;
        char ch;

        browsePocket(master);
        printf("Which pokemon you want to choose: ");
        scanf("%llu", &pocketID);
        printf("Pocket id: %llu, sure ? (y/n) ", pocketID);
        getchar();

        if ((ch = getchar()) == 'y' && pocketID < 0x8 && master->pocket[ pocketID-1 ]) {
            master->pocket[ pocketID-1 ]->hp += 999;
            printf("OK, hp of your pokemon is %u, recovering very well\n", master->pocket[ pocketID-1 ]->hp);
            printf("Use %s successfully\n", prop);
        } else {
            printf("Use %s unsuccessfully\n", prop);
            return;
        }
    } else if (strcmp(prop, "Kill Potion") == 0) {
        printf("Use \"Kill Potion\" to kill %s\n", monster->name);
        monster->hp = 0;
    } else {
        printf("Unknown prop %s\n", prop);
        return;
    }

    master->props[idx] = NULL;
    free(prop);
}

// --------menu action start--------
int fight(PokemonMaster *master, Pokemon *monster)
{
    puts("\n--------Monster Information--------");
    puts(monster->icon);
    printf("[name]  %s\n[id]    %u\n[hp]    %d\n[atk]   %u\n[speed] %u\n[type]  %s\n\n",
            monster->name, monster->id, monster->hp, monster->attack, monster->speed, TYPE[monster->type-1]);

    Pokemon *pokemon;
    unsigned int idx;

    browsePocket(master);
    printf("Select a pokemon: ");
    scanf("%u", &idx);

    if (idx < 8 && master->pocket[idx-1] != NULL) {
        pokemon = master->pocket[idx-1];
        printf("OK! Select %s\n", master->pocket[idx-1]->name);
        showPokemon(master, idx-1);
    } else {
        printf("Game over !");
        exit(1);
    }

    unsigned int opt;
    unsigned int speed = 0;
    unsigned int totalDamage = 0;
    int round = 1;

    sleep(1);
    while (pokemon->hp > 0) {
        cls();
        printf("\n------------Round %d------------\n", round++);
        printf("[%s] hp: %d\n", monster->name, monster->hp);
        printf("[%s] hp: %d\n", pokemon->name, pokemon->hp);
        printf("[%s] total damage: %u\n", pokemon->name, totalDamage);
        menu(pokemon);
        scanf("%u", &opt);

        switch (opt) {
            case 1:
                totalDamage += selectSkill(monster, pokemon, totalDamage);
                break;
            case 2:
                selectProp(master, monster);
                break;
            case 3:
                pokemon = selectPokemon(master);
                break;
            case 4:
                puts("Go away successfully !");
                sleep(1);
                return -1;
            default:
                puts("Unknown option !");
                exit(1);
                break;
        }

        if (monster->hp <= 0) {
            sleep(1);
            return 1;
        }

        if (pokemon->hp > 0) {
            if (speed >= 20)
                speed -= 20;
            else
                normalAtk(monster, pokemon);
        } else {
            return 0;
        }

        speed += pokemon->speed;
        sleep(1);
    }

    return 0;
}

unsigned int selectSkill(Pokemon *monster, Pokemon *pokemon, unsigned int damage)
{
    const char *skillAtkName;
    const char *skillBuffName;
    char ch;
    int verbosMode = 0;

    getchar();
    printf("Enter verbose mode ? (y/n) : ");
    if ((ch = getchar()) == 'y') {
        verbosMode = 1;
    }

    if (pokemon->skillAttack == VineWhip) {
        skillAtkName = "VineWhip";
    } else if (pokemon->skillAttack == RazorLeaf) {
        skillAtkName = "RazorLeaf";
    } else if (pokemon->skillAttack == FrenzyPlant) {
        skillAtkName = "FrenzyPlant";
    } else if (pokemon->skillAttack == Ember) {
        skillAtkName = "Ember";
    } else if (pokemon->skillAttack == FireFang) {
        skillAtkName = "FireFang";
    } else if (pokemon->skillAttack == Bubble) {
        skillAtkName = "Bubble";
    } else if (pokemon->skillAttack == AquaJet) {
        skillAtkName = "AquaJet";
    } else if (pokemon->skillAttack == HydroPump) {
        skillAtkName = "HydroPump";
    } else if (pokemon->skillAttack == Bite) {
        skillAtkName = "Bite";
    } else if (pokemon->skillAttack == ShadowBall) {
        skillAtkName = "ShadowBall";
    } else {
        skillAtkName = "";
    }

    if (pokemon->skillBuff == SpeedUp) {
        skillBuffName = "SpeedUp";
    } else if (pokemon->skillBuff == AttackUp) {
        skillBuffName = "AttackUp";
    } else if (pokemon->skillBuff == HPUp) {
        skillBuffName = "HPUp";
    } else if (pokemon->skillBuff == SuperHealing) {
        skillBuffName = "SuperHealing";
    } else {
        skillBuffName = "";
    }

    puts("|----------------------------------|");
    printf("|1. %-*s|2. %-*s|\n", 14, skillAtkName, 13, skillBuffName);
    puts("|----------------------------------|");

    unsigned int idx;
    
    printf("Which skill you want to choose: ");
    scanf("%u", &idx);

    if (idx != 1 && idx != 2) {
        puts("Invalid index");
        exit(1);
    }

    Pokemon *tmpPkm = pokemon;

    if (idx == 1) {
        unsigned int prevHp = monster->hp;

        if (verbosMode) {
            char buf[0x20] = {0};
            int bytes;
            
            printf("Current total damage: %u\n", damage);
            printf("Expected damage: ");
            scanf("%32s", buf);
            bytes = countBytes(buf, &damage); // overwrite monster pointer
            if (bytes > 4) {
                printf("[*] The damage %u is impossible !\n", damage);
            }
        }
        printf("Use %s !\n", skillAtkName);
        pokemon->skillAttack(tmpPkm, monster);
        if (verbosMode) {
            printf("Actual damage: %u\n", prevHp - monster->hp);
        }

        return prevHp - monster->hp;
    } else {
        if (verbosMode) {
            printf("**BEFORE**\n[hp]    %d\n[atk]   %u\n[speed] %u\n",
                    pokemon->hp, pokemon->attack, pokemon->speed);
        }
        printf("Use %s !\n", skillBuffName);
        pokemon->skillBuff(tmpPkm);
        if (verbosMode) {
            printf("**AFTER**\n[hp]    %d\n[atk]   %u\n[speed] %u\n",
                    pokemon->hp, pokemon->attack, pokemon->speed);
        }
    }

    return 0;
}

void selectProp(PokemonMaster *master, Pokemon *monster)
{
    int idx; // must be unsigned
    
    showProps(master);
    printf("Which prop you want to choose: ");
    scanf("%u", &idx);

    if ((idx >= 4 || idx <= 0) && master->props[idx-1] == NULL) { // logic error (&& -> ||)
        puts("Invalid index");
        exit(1);
    }

    useProp(master, monster, idx-1);
}

Pokemon *selectPokemon(PokemonMaster *master)
{
    unsigned int idx;

    browsePocket(master);
    printf("Which pokemon you want to choose: ");
    scanf("%u", &idx);

    if (idx >= 8 || master->pocket[idx-1] == NULL) {
        puts("Invalid index");
        exit(1);
    }
    printf("%s, I Choose You!\n", master->pocket[idx-1]->name);

    return master->pocket[idx-1];
}

void menu(Pokemon *pokemon)
{
    printf("\n\n\n|----------------------------|----------------------------|\n");
    printf("|  %-*s|  1. fight   |  2. backpack |\n", 26, pokemon->name);
    printf("|  What do you want to do ?  |----------------------------|\n");
    printf("|                            |  3. pokemon |  4. run away |\n");
    printf("|----------------------------|----------------------------|\n");
    printf("> ");
}

// --------menu action end--------

MonsterLoc *generateMonster(int idx, int rint)
{
    MonsterLoc *loc = (MonsterLoc *)malloc(sizeof(MonsterLoc));
    switch (rint) {
        case 0: loc->monster = Mewtwo(); break;
        case 1: loc->monster = Bulbasaur(); break;
        case 2: loc->monster = Ivysaur(); break;
        case 3: loc->monster = Venusaur(); break;
        case 4: loc->monster = Charmander(); break;
        case 5: loc->monster = Charmeleon(); break;
        case 6: loc->monster = Charizard(); break;
        case 7: loc->monster = Squirtle(); break;
        case 8: loc->monster = Wartortle(); break;
        case 9: loc->monster = Blastoise(); break;
        case 10: loc->monster = Meowth(); break;
    }

    switch (idx) {
        case 0: loc->x = 10; loc->y = 1; break;
        case 1: loc->x = 21; loc->y = 4; break;
        case 2: loc->x = 3; loc->y = 5; break;
        case 3: loc->x = 8; loc->y = 2; break;
        case 4: loc->x = 29; loc->y = 3; break;
        case 5: loc->x = 34; loc->y = 6; break;
    }

    return loc;
}

Pokemon *Bulbasaur() // 妙蛙種子
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Bulbasaur");
    pokemon->id = 1;
    pokemon->hp = 100;
    pokemon->attack = 10;
    pokemon->speed = 3;
    pokemon->type = TYPE_GRASS;
    pokemon->skillAttack = VineWhip;
    pokemon->skillBuff = SpeedUp;
    pokemon->level = 1;
    pokemon->icon = iconBlastoise;

    return pokemon;
}

Pokemon *Ivysaur() // 妙蛙草
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Ivysaur");
    pokemon->id = 2;
    pokemon->hp = 130;
    pokemon->attack = 15;
    pokemon->speed = 8;
    pokemon->type = TYPE_GRASS;
    pokemon->skillAttack = RazorLeaf;
    pokemon->skillBuff = SpeedUp;
    pokemon->level = 2;
    pokemon->icon = iconIvysaur;

    return pokemon;
}

Pokemon *Venusaur() // 妙蛙花
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Venusaur");
    pokemon->id = 3;
    pokemon->hp = 180;
    pokemon->attack = 25;
    pokemon->speed = 17;
    pokemon->type = TYPE_GRASS;
    pokemon->skillAttack = FrenzyPlant;
    pokemon->skillBuff = SpeedUp;
    pokemon->level = 3;
    pokemon->icon = iconVenusaur;

    return pokemon;
}

Pokemon *Charmander() // 小火龍
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Charmander");
    pokemon->id = 4;
    pokemon->hp = 101;
    pokemon->attack = 20;
    pokemon->speed = 1;
    pokemon->type = TYPE_FIRE;
    pokemon->skillAttack = Ember;
    pokemon->skillBuff = AttackUp;
    pokemon->level = 1;
    pokemon->icon = iconCharmander;

    return pokemon;
}

Pokemon *Charmeleon() // 火恐龍
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Charmeleon");
    pokemon->id = 5;
    pokemon->hp = 134;
    pokemon->attack = 33;
    pokemon->speed = 4;
    pokemon->type = TYPE_FIRE;
    pokemon->skillAttack = FireFang;
    pokemon->skillBuff = AttackUp;
    pokemon->level = 2;
    pokemon->icon = iconCharmeleon;

    return pokemon;
}

Pokemon *Charizard() // 噴火龍
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Charizard");
    pokemon->id = 6;
    pokemon->hp = 188;
    pokemon->attack = 50;
    pokemon->speed = 9;
    pokemon->type = TYPE_FIRE;
    pokemon->skillAttack = Flamethrower;
    pokemon->skillBuff = AttackUp;
    pokemon->level = 3;
    pokemon->icon = iconCharizard;

    return pokemon;
}

Pokemon *Squirtle() // 傑尼龜
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Squirtle");
    pokemon->id = 7;
    pokemon->hp = 130;
    pokemon->attack = 11;
    pokemon->speed = 1;
    pokemon->type = TYPE_WATER;
    pokemon->skillAttack = Bubble;
    pokemon->skillBuff = HPUp;
    pokemon->level = 1;
    pokemon->icon = iconSquirtle;

    return pokemon;
}

Pokemon *Wartortle() // 卡咪龜
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Wartortle");
    pokemon->id = 8;
    pokemon->hp = 174;
    pokemon->attack = 15;
    pokemon->speed = 3;
    pokemon->type = TYPE_WATER;
    pokemon->skillAttack = AquaJet;
    pokemon->skillBuff = HPUp;
    pokemon->level = 2;
    pokemon->icon = iconWartortle;

    return pokemon;
}

Pokemon *Blastoise() // 水箭龜
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Blastoise");
    pokemon->id = 9;
    pokemon->hp = 230;
    pokemon->attack = 23;
    pokemon->speed = 7;
    pokemon->type = TYPE_WATER;
    pokemon->skillAttack = HydroPump;
    pokemon->skillBuff = HPUp;
    pokemon->level = 3;
    pokemon->icon = iconBlastoise;

    return pokemon;
}

Pokemon *Meowth() // 喵喵
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Meowth");
    pokemon->id = 10;
    pokemon->hp = 99999999;
    pokemon->attack = 99999999;
    pokemon->speed = 99999999;
    pokemon->type = TYPE_NORMAL;
    pokemon->skillAttack = Bite;
    pokemon->skillBuff = SuperHealing;
    pokemon->level = 99;
    pokemon->icon = iconMeowth;

    return pokemon;
}

Pokemon *Mewtwo() // 超夢
{
    Pokemon *pokemon = (Pokemon *)malloc(sizeof(Pokemon));
    memset(pokemon->name, 0, 0x30);
    strcpy(pokemon->name, "Mewtwo");
    pokemon->id = 11;
    pokemon->hp = 800;
    pokemon->attack = 130;
    pokemon->speed = 30;
    pokemon->type = TYPE_PSYCHIC;
    pokemon->skillAttack = VineWhip;
    pokemon->skillBuff = SuperHealing;
    pokemon->level = 99;
    pokemon->icon = iconMewtwo;

    return pokemon;
}

void normalAtk(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack;
    printf("%s got damage %u\n", defender->name, attacker->attack);
}

void VineWhip(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*2;
    printf("%s got damage %u\n", defender->name, attacker->attack*2);
}

void RazorLeaf(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*3;
    printf("%s got damage %u\n", defender->name, attacker->attack*3);
}

void FrenzyPlant(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack;
    printf("%s got damage %u\n", defender->name, attacker->attack);
}

void Ember(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*2;
    printf("%s got damage %u\n", defender->name, attacker->attack*2);
}

void FireFang(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*3;
    printf("%s got damage %u\n", defender->name, attacker->attack*3);
}

void Flamethrower(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack;
    printf("%s got damage %u\n", defender->name, attacker->attack);
}

void Bubble(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*2;
    printf("%s got damage %u\n", defender->name, attacker->attack*2);
}

void AquaJet(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*3;
    printf("%s got damage %u\n", defender->name, attacker->attack*3);
}

void HydroPump(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack;
    printf("%s got damage %u\n", defender->name, attacker->attack);
}

void Bite(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*2;
    printf("%s got damage %u\n", defender->name, attacker->attack*2);
}

void ShadowBall(Pokemon *attacker, Pokemon *defender)
{
    defender->hp -= attacker->attack*3;
    printf("%s got damage %u\n", defender->name, attacker->attack*3);
}

void SpeedUp(Pokemon *self)
{
    self->speed += self->level;
    printf("%s up speed %u\n", self->name, self->level);
}

void AttackUp(Pokemon *self)
{
    self->attack += self->level*2;
    printf("%s up attack %u\n", self->name, self->level*2);
}

void HPUp(Pokemon *self)
{
    self->hp += self->level*10;
    printf("%s up hp %u\n", self->name, self->level*10);
}

void SuperHealing(Pokemon *self)
{
    self->hp += 999;
    printf("%s recover all hp\n", self->name);
}