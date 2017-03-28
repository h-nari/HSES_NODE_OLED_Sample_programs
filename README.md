# HSES_NODE_OLED_sample_programs

* Sample programs for HSES-NODE-OLED (ESP8266 + OLED)
* ハンブルソフト製<a href="http://www.humblesoft.com/products/HSES-NODE/">HSES-NODE-OLED</a>用サンプルプログラム集です。
* ESP8266 Arduino用のスケッチになっています。
* インストール方法は次のページをご覧ください→[HSES_NODE_OLED_Sample_programs Install](http://www.humblesoft.com/wiki/?HSES_NODE_OLED_Sample_programs%20Install "HSES_NODE_OLED_Sample_programs Install")

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

## RssDisp
<a href="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170327b1.jpg?raw=true"><img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170327b1.jpg?raw=true" align="left" hspace="40" width="300" /></a>
* RSS(Webサイトの更新情報)を表示
* 日付、時刻も表示
* スイッチ(IO0)押下でレイアウト変更

* 動作デモ動画:   <a href="https://youtu.be/TxDbCP7rE2w"> https://youtu.be/TxDbCP7rE2w</a>


<br clear="left">

## NetOLED

<table><tr><td>
<a href="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170301a3.jpg?raw=true"><img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170301a3.jpg?raw=true" hspace="40" width="300" /></a>
</td><td>
 <a href="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170301a5.jpg?raw=true"><img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/170301a5.jpg?raw=true" hspace="40" width="300" /></a>
</td></tr></table>

* UDPパケットで画像情報を受け取り、OLEDに表示するプログラム
* PC側でPython等で画像を作成し、送信
 * Python用ライブラリを提供、Python画像ライブラリPillowの画僧を、そのまま表示可能
* <a href="https://youtu.be/DZ9h0Leanrk">添付サンプルプログラム dispDemo.py実行動画</a>
 * https://youtu.be/DZ9h0Leanrk
* <a hef="https://youtu.be/rIRIXXnOoCE">添付サンプルプログラム dispImage.py実行動画</a>
 * https://youtu.be/rIRIXXnOoCE
