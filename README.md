# HSES_NODE_OLED_sample_programs

* Sample programs for HSES-NODE-OLED (ESP8266 + OLED)
* ハンブルソフト製<a href="http://www.humblesoft.com/products/HSES-NODE/">HSES-NODE-OLED</a>用サンプルプログラム集です。
* ESP8266 Arduino用のスケッチになっています。

## analog clock

<a href="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170123b2.jpg?raw=true"><img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170123b2.jpg?raw=true" align="left" hspace="40" width="300"/></a>

* ntp 同期アナログ時計
* グラフィック表示(長針、短針、秒針)
* 日付、時:分表示
<br clear="left" />



## digital clock

<a href="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170123b0.jpg?raw=true"><img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170123b0.jpg?raw=true" align="left" hspace="40" width="300" /></a>
* ntp同期ディジタル時計
* 西暦、和暦、日付、曜日表示
* 時:分:秒 表示
<br clear="left"/>

## NetOLED

* UDPパケットで画像情報を受け取り、OLEDに表示するプログラム
* PC側でPython等で画像を作成し、送信
 * Python用ライブラリを提供、Python画像ライブラリPillowの画僧を、そのまま
 表示可能

 * 添付サンプルプログラム dispDemo.py実行動画

<iframe width="560" height="315" src="https://www.youtube.com/embed/DZ9h0Leanrk" frameborder="0" allowfullscreen></iframe>


* 添付サンプルプログラム dispImage.py実行動画

<iframe width="560" height="315" src="https://www.youtube.com/embed/rIRIXXnOoCE" frameborder="0" allowfullscreen></iframe>
