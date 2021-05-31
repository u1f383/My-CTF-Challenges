#pragma once

typedef struct _Pokemon
{
    char name[0x30]; // "/bin/sh"

    void (*skillAttack)(struct _Pokemon *pokemon, struct _Pokemon *monster);
    void (*skillBuff)(struct _Pokemon *self);

    int hp;
    unsigned int id;
    unsigned int attack;
    unsigned int speed;
    unsigned int type;

    const char *icon;
    unsigned int level;
} Pokemon;

typedef struct _PokemonMaster
{
    char *props[4];
    char pokedex[0x10];
    char name[0x30];
    int pokemonNum;
    Pokemon *pocket[8];
} PokemonMaster;

typedef struct _MonsterLoc
{
    Pokemon *monster;
    int x;
    int y;
} MonsterLoc;

MonsterLoc *generateMonster(int idx, int rint);
PokemonMaster *newPokemonMaster();
Pokemon *findPokemon(PokemonMaster *master, unsigned int id);
void catchPokemon(PokemonMaster *master, Pokemon *pokemon);
void releasePokemon(Pokemon *pokemon);
void showPokemon(PokemonMaster *master, unsigned int idx);
void showPokedex(PokemonMaster *master);
void showPocket(PokemonMaster *master);
void showProps(PokemonMaster *master);
void browsePocket(PokemonMaster *master);

int fight(PokemonMaster *master, Pokemon *monster);
int win(PokemonMaster *master);
int countBytes(char *buf, char *damage);
void menu(Pokemon *pokemon);
void getProp(PokemonMaster *master);
void selectProp(PokemonMaster *master, Pokemon *monster);
unsigned int selectSkill(Pokemon *monster, Pokemon *pokemon, unsigned int damage);
Pokemon *selectPokemon(PokemonMaster *master);

Pokemon *Bulbasaur(); // 妙蛙種子
Pokemon *Ivysaur(); // 妙蛙草
Pokemon *Venusaur(); // 妙蛙花
Pokemon *Charmander(); // 小火龍
Pokemon *Charmeleon(); // 火恐龍
Pokemon *Charizard(); // 噴火龍
Pokemon *Squirtle(); // 傑尼龜
Pokemon *Wartortle(); // 卡咪龜
Pokemon *Blastoise(); // 水箭龜
Pokemon *Meowth(); // 喵喵
Pokemon *Mewtwo(); // 超夢

void normalAtk(Pokemon *attacker, Pokemon *defender);
void VineWhip(Pokemon *attacker, Pokemon *defender);
void RazorLeaf(Pokemon *attacker, Pokemon *defender);
void FrenzyPlant(Pokemon *attacker, Pokemon *defender);
void Ember(Pokemon *attacker, Pokemon *defender);
void FireFang(Pokemon *attacker, Pokemon *defender);
void Flamethrower(Pokemon *attacker, Pokemon *defender);
void Bubble(Pokemon *attacker, Pokemon *defender);
void AquaJet(Pokemon *attacker, Pokemon *defender);
void HydroPump(Pokemon *attacker, Pokemon *defender);
void Bite(Pokemon *attacker, Pokemon *defender);
void ShadowBall(Pokemon *attacker, Pokemon *defender);
void SpeedUp(Pokemon *self);
void AttackUp(Pokemon *self);
void HPUp(Pokemon *self);
void SuperHealing(Pokemon *self);
