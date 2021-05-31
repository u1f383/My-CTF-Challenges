# pokemon

## 考點

1. 逆向程式的能力
2. fmt trick
3. OOB
4. partial overwrite
5. 串接可以使用的 struct element

## Exploit

> 這題是我這次 AIS3 花最多時間出的一題 XD

題目在 release 不久後就有更新 distribution，其中有放 source code，如果直接編譯的話應該會看到下面的 warning：
``` shell
$ make      
gcc -g -o pokemon pokemon.c pokedex.c -lcurses
pokedex.c: In function ‘selectSkill’:
pokedex.c:369:37: warning: passing argument 2 of ‘countBytes’ from incompatible pointer type [-Wincompatible-pointer-types]
  369 |             bytes = countBytes(buf, &damage); // overwrite monster pointer
      |                                     ^~~~~~~
      |                                     |
      |                                     unsigned int *
pokedex.c:137:33: note: expected ‘char *’ but argument is of type ‘unsigned int *’
  137 | int countBytes(char *buf, char *damage)
      |                           ~~~~~~^~~~~~
```

而應該可以猜到有 warning 的地方就有可能會出現一些問題，因此之後在追 code 時， `countBytes()` 可以優先分析。

我預期的 exploit 過程如下：
1. leak code
2. leak libc + leak heap (optional)
3. partial overwrite
4. get shell

而我不會講解整個遊戲的運作流程，不過大致上就跟大家印象中的寶可夢一樣，這邊就不暴雷，留給大家去品嘗。

### 1. leak code

在 pokedex.c 的 154 行，有一個 `scanf("%llu")`：
``` c
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
        scanf("%llu", &pocketID); <--- here
        printf("Pocket id: %llu, sure ? (y/n) ", pocketID);
        getchar();
    ...
 ```

 `scanf()` 也是 fmt 系列的 function 之一，而當他預期讀時 ("%llu", "%d")，如果我們輸入的是 '+' or '-'，會被用來[判斷 sign 而不當作 integer 本身](https://elixir.bootlin.com/glibc/latest/source/stdio-common/vfscanf-internal.c#L1397)，因此在這邊我們如果輸入 '+' or '-'，就能讀到沒有初始化的 `pocketID`，而在 stack 中很常會有殘留的 libc 或是 code address，這邊 `pocketID` 是存 code address，因此我們到這邊會有 **code base**。

### 2. leak libc

在 pokedex.c 的 397 行有一個邏輯錯誤的漏洞，這邊的邏輯是要檢查有沒有 `(idx >= 4 || idx <= 0)` **or** `master->props[idx-1] == NULL` 的狀況發生，如果發生的話代表是非法的存取。 然而這邊的 **or** 卻寫成 **and**，變成是 idx invalid 與對應位置的 value 為 NULL 同時發生才會判斷為錯。

``` c
void selectProp(PokemonMaster *master, Pokemon *monster)
{
    int idx;
    
    showProps(master);
    printf("Which prop you want to choose: ");
    scanf("%u", &idx);

    if ((idx >= 4 || idx <= 0) && master->props[idx-1] == NULL) { <--- here
        puts("Invalid index");
        exit(1);
    }

    useProp(master, monster, idx-1);
}
```

而此漏洞搭配 `useProp()` 的程式邏輯，可以達到任意 memory leak：
``` c
// pokedex.c L154
void useProp(PokemonMaster *master, Pokemon *monster, int idx)
{
    char *prop = master->props[idx];

    ...
    } else if (strcmp(prop, "Kill Potion") == 0) {
        printf("Use \"Kill Potion\" to kill %s\n", monster->name);
        monster->hp = 0;
    } else {
        printf("Unknown prop %s\n", prop); <--- here
        return;
    }
    ...
```

而由於 PokemonMaster 的 name 可以控制 (遊戲機制)，所以這邊可以把 name 改成 `stdin` 的 address，這樣 prop 就會指向 `stdin`，並在 `useProp()` 的最後印出 libc address。
``` c
// pokedex.h L20
typedef struct _PokemonMaster
{
    char *props[4];
    char pokedex[0x10];
    char name[0x30]; <--- here
    int pokemonNum;
    Pokemon *pocket[8];
} PokemonMaster;
```

TODO. 在這邊也能透過一樣的手段去 leak heap address，在之後的 exploit 就不用撞 1/16 了。

### 3. partial overwrite

有了 libc + code，剩下就是要找可以寫的地方，而一開始編譯時 `countBytes()` 有個 warning，可以看一下那個 function 的行為：
``` c
// pokedex.c L137
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
```

看起來是在計算以 `char *` type 存在的整數總共有幾個 byte，而該 function 的 caller 正常使用的話應該不會有 warning，因此我們回去看造成 warning 發生的地方：
```c
// pokedex.c L369
unsigned int selectSkill(Pokemon *monster, Pokemon *pokemon, unsigned int damage)
{
	...
    Pokemon *tmpPkm = pokemon;

    if (idx == 1) {
        unsigned int prevHp = monster->hp;

        if (verbosMode) {
            char buf[0x20] = {0};
            int bytes;
            
            printf("Current total damage: %u\n", damage);
            printf("Expected damage: ");
            scanf("%32s", buf);
            bytes = countBytes(buf, &damage); <--- here
            if (bytes > 4) {
                printf("[*] The damage %u is impossible !\n", damage);
            }
        }
        printf("Use %s !\n", skillAtkName);
        pokemon->skillAttack(tmpPkm, monster);
```

TODO. 我預期在寫 writeup 時發現 buf 竟然能夠讀 0x20 (最多 14 bytes)，我預期的是 0x10 (最多 7 bytes)，這邊先介紹 buf 只能讀 0x10 的 case，0x20 應該打法更多。

可以看到 `buf` 讀 0x10 大小的字串，如果轉換成 byte 的話，最多可以到 14 個 bytes，但是 `damage` 應該只會有 4 bytes 的大小 (unsigned int)，所以這邊有個可以 overflow 的地方，能蓋到 `pokemon` 但不能蓋壞掉，因為之後 `pokemon` 會 call `pokemon->skillAttack()`。

當可以 overflow 但是只能蓋掉幾個 bytes 時，就會想到 partial overwrite，在這邊我們把 `pokemon` 蓋成是同樣在 heap 上的 PokemonMaster，Why? 我們看看 Pokemon 跟 PokemonMaster 的 struct：
```c
typedef struct _Pokemon
{
    char name[0x30];
    void (*skillAttack)(struct _Pokemon *pokemon, struct _Pokemon *monster); <--- here
    void (*skillBuff)(struct _Pokemon *self);
    ...
} Pokemon;

typedef struct _PokemonMaster
{
    char *props[4];
    char pokedex[0x10];
    char name[0x30]; <--- here
    ...
} PokemonMaster;
```

最後的串接手法：
1. 可以看到 Pokemon.skillAttack 對到的是 PokemonMaster.name，因此 `pokemon->skillAttack(tmpPkm)` 會變成是 `master->name(tmpPkm)`
2. PokemonMaster.name 又可控 (遊戲機制)，因此將 `master->name(tmpPkm)` 改成 `name->system(tmpPkm) == system(tmpPkm)`
3. 第一個參數 `tmpPkm` 指向原本的 `pokemon`，因此又能看成 `system(pokemon)`
4. 最後，`Pokemon` 的第一個 member 是一個 char array，如果我們透過遊戲機制或是其他方法，把 `pokemon` 名字改成是 "/bin/sh"，就能當作這整個 function 等同於 `system("/bin/sh")`

[exp.py](exp.py)