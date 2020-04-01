# RssDisp: RSS表示プログラム

RSS(Webサイトの更新情報)を表示するプログラムです。

ニュースサイトのRSSを表示することで、
ニュースの電光掲示板のような表示ができます。

読み込むRSSは、ソースプログラム中に指定されていますので、
変更するには、プログラムを修正し書き込む必要があります。
変更するのは、 RssDisp.inoの61行目にある site配列の初期値です。

    const site_t site[] = {
        {"Yahoo Sports", 10, "https://news.yahoo.co.jp/pickup/sports/rss.xml",
         yahoo_fingerprint},
             ... 中略 ...
        {"asahi", 10, "https://rss.asahi.com/rss/asahi/newsheadlines.rdf",
         "5a5e16a96325bb81694d2b8e6f302a2e0945ee17"},
        {NULL},
    };

最初の要素は RSSのサイトの名前です。
2番目は表示される記事の最大数、3番めはRSSのurl、
4番目の要素はサイトの fingerprint(指紋)と呼ばれる文字列です。

サイトの名前は、プログラム中で、そのサイトを表すために使われます。

記事の最大数は、RSSに大量の記事、50記事とか含まれているが、
あまり古い記事まで表示させたくない場合などで指定します。
-1を指定すれば、メモリーの許す限りすべての記事を表示します。

RSSのURLは、以前と比べて探しづらくなりましたが、
<a href="https://www.google.com/search?q=RSS%E3%81%AEURL%E3%81%AE%E5%8F%96%E5%BE%97%E6%96%B9%E6%B3%95">
「RSSのURLの取得方法」で検索
</a>などして調べてください。
ただし、RssDispでは、URLが https:で始まるものしか使用できなく
なりましたので注意してください。
<a href="https://headlines.yahoo.co.jp/rss/list">Yahoo!ニュース - RSS</a>には、ニュースのRSSがまとめてあって便利です。

fingerprintはRSSを提供するホストの正当性を確認するための
データで、esp8266でhttpsのサイトにアクセスするのに必要です。
このデータは、 chromeでサイトにアクセスし、URLの左の鍵マークを
クリックし「証明書」を選択します。

<img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/sc200401d.PNG?raw=true">


表示される「証明書」ウィンドウで
詳細タブを選択し、フィールドで「拇印」をクリクすると
Fingerprintが表示されます。

<img src="https://github.com/h-nari/HSES_NODE_OLED_Sample_programs/blob/master/img/sc200401e.PNG?raw=true">

サイトの証明書の有効期限は1年程度で、
その後証明書が更新されると
fingerprintも変更されてしまいますので、
fingerprintの値を更新し、プログラムを修正する必要があります。
