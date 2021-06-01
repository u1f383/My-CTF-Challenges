# [祝賀] 新垣結衣新婚快樂 發錢囉 (myfirst only)

## 考點

1. U202E 混淆 (不影響答案)
2. zip 偽加密
3. png 隱寫 zip

## Exploit

題目附件只有一張圖，打開來只有三種不同風格的新垣結衣，如果你細心一點的話，可以看到在最左邊結衣的頭髮上有留 `AIS3{NOT_ME^_^}` 的假 flag。

當圖片表面沒有資訊後，就會開始查看圖片背後是否藏些什麼資訊。 而分析檔案內容的工具有很多種，在這邊舉兩個為例子： `string` 以及 `binwalk` 。

使用 `string` 會看到輸出的結尾有 .png 的字串，而使用 `binwalk` 的話會發現它告訴你這張照片裡面有 zip 的資料，到這邊可以判斷出這張圖片藏了其他檔案。

之後 google 搜尋 `linux hide zip in png` 之類的關鍵字，就會發現其實在 png 裡面藏資料只需要 `cat my.png secret.zip >> secret.png` ，就可以把 `secret.zip` 檔案內的資料藏在 `secret.png` 中，並且呈現的圖片為 `my.png`，這就是一個常見的隱寫方法 [(參考連結](https://wiki.linuxquestions.org/wiki/Embed_a_zip_file_into_an_image)

有很多種方法可以 extract 出 secret：
1. 如果用手動的話，必須想辦法找到 my.png 的結尾或是 secret.zip 的開頭，之後把 my.png 刪掉，就會留下 secret.zip 了
2. 如果用工具的話可以選 `binwalk` 
3. 因為 flag.png 檔案內部塞了一個 zip， google 一下就能知道用 `unzip secret.png` 就能解壓縮 secret.png 內的 secret.zip，不需要 extract 出來

當有辦法存取到 secret.png 內藏的 secret.zip 後，下一步就會想解壓縮他，但是解壓縮的過程中他會要求輸入密碼，不能順利解壓縮。最直接想到的是 brute force 爆破密碼，但是 MyFirst 這麼短，考這個也沒啥意義，因此基本上可以排除此可能。

沒有密碼但是 zip 要你輸入密碼才能解壓縮，這就是 [zip 偽加密](https://www.programmersought.com/article/33704714691/)，原因是 zip 結構中有一些資料是紀錄檔案是否加密，如果手動調整那些資料，就能讓 zip 相關工具以為此 .zip 檔需要輸入密碼才能夠解壓縮，要解決此問題就是將資料調回來。

不過偽加密的 .zip 檔其實會被認定是 corrupted，因此如果用 zip 提供 `-FF` (`--fixfix`) 的 flag 來修復此 .zip 檔案就能回復成未加密的樣子。

解開後會有一個檔案名稱為 `awertyjasgnp.knmrq` 的檔案，其實是檔案名稱 `awertyjasqrmnk.png` 中加入了 `U202E` unicode (`awertyjas<U202E>qrmnk.png`)，此 unicode 會讓後面的字串以倒著的方式呈現，因此會讓此 .png 看起來不像 .png。

最後打開 png 就是 flag 了： `AIS3{看好了世界，我只示範一次如何一天搶走幾億人的老婆}`
