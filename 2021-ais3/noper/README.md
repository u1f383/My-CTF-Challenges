# noper

## 考點
1. `rand()` 在沒有 `srand()` 更改 seed 的情況下，產生的值是固定的
2. 該如何自己手動調整 shellcode payload

## Exploit

這題的打法非常值觀，就是讀使用者的一段 shellcode，然後中間幾個 byte 被取代成 nop ，最後會直接將這串 shellcode 拿來執行，所以要做的事情是想辦法 bypass 掉那些 nop，構造出可以呼叫 `sys_execve("/bin/sh", 0, 0)` 的 payload。 而 banner 雖然顯示 "Noper is tool can replace your code with nop randomly"，不過可以看到程式使用 `rand()` 時並沒有初始化 seed，因此每次 `rand()` 的 sequence 都會是一樣的，只需要將會被取代成 nop 的 byte 的位置記錄下來，避免將 shellcode 寫入那些位置就夠了。

[exp.py](exp.py)