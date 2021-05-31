# sleeper

## 考點

1. OOB 蓋寫其他欄位
2. 使用工具 patch byte，加快 debug 速度

## Exploit

首先瀏覽一下整個程式的運作：
1. 新建一個 Timer 變數 t，並且 moretimes 為 8 * 60 * 60
2. 等待 10 秒後讀一個整數，並傳入 `sleep()`
3. 當 `sleep()` 結束後，會根據 moretimes 再 `sleep()` 一次
4. 當該次 `sleep()` 結束，就能成功拿到 shell

程式只有一個輸入點：
``` c
fgets(t.buf, 0x10, stdin);
```

並且觀察一下 Timer 的 struct：
``` c
typedef struct _Timer {
    char buf[8];
    unsigned int times;
    unsigned int moretimes;
} Timer;
```

可以發現 buf size 只有 8，但 `fgets()` 讀進來卻是 0x10，有一個明顯的 overflow，而超過 0x8 的部分會蓋到 t.moretimes。 因此我們可以透過寫一堆 0x00 到 t.moretimes，把原本的初始值 8 * 60 * 60 蓋成 0，讓 `sleep(t.moretimes)` 馬上結束。

[exp.py](exp.py)