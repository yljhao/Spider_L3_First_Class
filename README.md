Spider_L3_First_Class
=================================================================================

你也可以"物聯網"一下工作坊的課程資料，包括

1. Spider L3S Library
2. Practice 1 ~ Practice 4的課程用程式
3. Spider L3S FW 1.12的更新檔案
4. 外殼雷切檔案
5. 課程投影片請至連結：

https://drive.google.com/file/d/0By0AKx4nVPKfbTBmbnZTWjFCaTA/edit?usp=sharing

若有發現Bug的話會持續更新，請上過課的學員可以注意一下，
另外也請學員們將手上Spider L3S模組做韌體更新，
這樣WiFi開關在運作時，動作會比較正常。

=================================================================================

更新韌體的方式：

1. 參考課程投影片，下載右下角.zip檔案並且壓縮
2. 刪除資料夾名稱的-master字尾
3. 開啟Arduino IDE並且將sketchbook location，
   設定到剛剛解壓縮的資料夾下，並重新開啟Arduino IDE
4. 載入Patch_1_12.ino，並且將code燒入Arduino
5. 接上WiFi shield，開啟Serial monitor，設定Baudrate 115200
   與Line ending為Both NL & CR
6. 輸入0確認WiFi module連接是否正常，並且確認韌體版本為1.28
7. 確認Arduino供電正常，周邊連接穩固後，輸入AY自動更新韌體
8. 更新韌體完後輸入0確認韌體版本為1.26
9. 更新完成

=================================================================================

如何更換圖片或者網頁?

1. 請至此下載bin2h.exe小程式 => https://code.google.com/p/bin2h/
2. 將想轉換成二進位的圖片或者網頁與bin2h放到同一個資料夾下
3. 打開命令列，輸入"bin2h.exe file_input.html file_output.h"
4. 打開同資料夾下的file_output.h檔案，並且將C陣列資料貼到Arduino程式內的C陣列上
5. 重新下載檔案檢查是否修改成功。

