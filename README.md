# roomWatching
## 概要
- M5Stackを利用して、気温湿度気圧を計測する
- LINE Notify を通じて計測結果を通知する(https://notify-bot.line.me/ja/)
- Ambientで計測データをグラフ化(https://ambidata.io/)
## 使い方
1. `src/Config.hpp.sample`を複製
2. 複製したファイルを`src/Config.hpp`にリネーム
3. `src/Config.hpp`の定数設定
```
#define AMBINET_CHANNEL_ID 00000  // AmbientのチャネルID
#define AMBINET_WRITE_KEY "XXX" // Ambientのライトキー
#define SSID "XXX" // 接続するWIFIのSSID
#define PASS "XXX" // 接続するWIFIのパスワード
#define LINE_HOST "notify-api.line.me" // LINE-Notifyのホスト
#define LINE_KEY "XXX" // LINE-NotifyのAPIキー
```
