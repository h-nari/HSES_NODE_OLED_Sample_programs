#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>

#include "conf.h"

#if USE_OTA
#include <ArduinoOTA.h>

#include "ota_util.h"
#endif

#include <Adafruit_GFX.h>
#include <Fontx.h>
#include <Humblesoft_GFX.h>
#include <Humblesoft_SSD1306.h>
#include <ScrollText.h>
#include <fontx/ILGH16XB.h>
#include <fontx/ILGZ16XB.h>

#include "layout.h"
#include "rssBuf.h"
#include "rssParser.h"

#define DAYSEC (3600 * 24)

#define SW_PIN 0
#define OLED_DC 15
#define OLED_RESET 16
#define OLED_CS 2

void datetime_disp(LayoutElem *elem, bool bInit);
void date_disp(LayoutElem *elem, bool bInit);
void time_disp(LayoutElem *elem, bool bInit);
void rss_disp(LayoutElem *elem, bool bInit);
void rss_aux_disp(LayoutElem *elem, bool bInit);

enum AppState {
  AS_READ,
  AS_DISP,
};

enum SW_STATE {
  SWS_Pushed,
  SWS_Released,
  SWS_Stabled,
};

typedef struct {
  const char *name;
  int max;
  const char *url;
  const char *figerprint;
} site_t;

const char *yahoo_fingerprint =
    "1E8BB7F438189245FCC779DD4DA9912BBC2F70CF";  // SHA-1 until 2021/8/27
const char *impress_fingerprint =
    "01A4812C19BE0854B91500A627CC17796F4F4CDE";  // SHA-1 until 2022//30
const char *jiji_fingerprint =
    "F9F5CB7D14E5B4A104352DD9670301A1BF350E10";  // SHA-1 until 2020/12/8
const char *asahi_fingerprint =
    "921342996D60845FDC1BCE7E9F700D20047A3E0F";// SHA-1 until 2021/9/5

const site_t site[] = {
    {"Yahoo Sports", 10, "https://news.yahoo.co.jp/pickup/sports/rss.xml",
     yahoo_fingerprint},
    {"Yahoo IT", 10, "https://news.yahoo.co.jp/pickup/computer/rss.xml",
     yahoo_fingerprint},
    {"sankei", 10, "https://headlines.yahoo.co.jp/rss/san-dom.xml",
     yahoo_fingerprint},
    {"Impress", 10, "https://www.watch.impress.co.jp/data/rss/1.0/ipw/feed.rdf",
     impress_fingerprint},
    {"jiji", 10, "https://www.jiji.com/rss/ranking.rdf", jiji_fingerprint},
    {"asahi", 10, "https://rss.asahi.com/rss/asahi/newsheadlines.rdf",
     asahi_fingerprint},
    {NULL},
};

Layout layouts[] = {{
                        {
                            {0, 0, 128, 16, datetime_disp},
                            {0, 16, 128, 16, rss_aux_disp},
                            {0, 32, 128, 32, rss_disp},
                        },
                    },
                    {
                        {
                            {0, 0, 128, 16, date_disp},
                            {0, 16, 128, 16, time_disp},
                            {0, 32, 128, 16, rss_aux_disp},
                            {0, 48, 128, 16, rss_disp},
                        },
                    },
                    {
                        {
                            {0, 0, 128, 16, date_disp},
                            {0, 16, 128, 16, time_disp},
                            {0, 32, 128, 32, rss_disp},
                        },
                    },
                    {
                        {
                            {0, 0, 128, 16, date_disp},
                            {0, 16, 128, 32, time_disp},
                            {0, 48, 128, 16, rss_disp},
                        },
                    },
                    {
                        {
                            {0, 0, 128, 32, date_disp},
                            {0, 32, 128, 16, time_disp},
                            {0, 48, 128, 16, rss_disp},
                        },
                    }};

AppState gState;
int httpCode;
uint8_t iItem;
uint8_t iLayout;

RssBuf rssBuf;
RssParser rss;
const site_t *pSite = site;

SW_STATE sw_state = SWS_Stabled;
unsigned long sw_release_time;

RomFontx fontx(ILGH16XB, ILGZ16XB);
Humblesoft_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);
ScrollText scroll(&oled);
ScrollText sc_aux(&oled);

unsigned long tStart;

static void site_next() {
  pSite++;
  if (pSite->name == NULL) {
    unsigned long tNow = millis();
    pSite = site;
    Serial.printf("loop time: %d sec\n", (tNow - tStart) / 1000);
  }
}

void rss_update() {
  if (gState == AS_READ) {
    rssBuf.clear();
    rssBuf.setItemMax(pSite->max);
    rss.init();

    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    HTTPClient http;

    if (pSite->figerprint) {
      client->setFingerprint(pSite->figerprint);
    }
    if (!http.begin(*client, pSite->url)) {
      Serial.printf("%s connection failed.\n", pSite->name);
      scroll.printf("[[%sへの接続に失敗しました.]]  ", pSite->name);
      site_next();
    } else {
      Serial.printf("%s connected\n", pSite->name);

      httpCode = http.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTP] -> code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          int len = http.getSize();
          Serial.printf("len:%d\n", len);
          uint8_t buff[128] = {0};
          WiFiClient *stream = http.getStreamPtr();
          unsigned long tStart = millis();
          while (http.connected() && (len > 0 || len == -1)) {
            size_t size = stream->available();
            if (size) {
              if (size > sizeof buff) size = sizeof buff;
              int c = stream->readBytes(buff, size);
              for (int i = 0; i < c; i++) rss.put(buff[i]);
              if (len > 0) len -= c;
              tStart = millis();
            } else if (millis() - tStart > 5000) {
              break;
            } else if (rssBuf.isFull()) {
              break;
            }
            layout_update(&layouts[iLayout], rss_disp);
          }
          Serial.printf("[HTTP] ItemCount:%d\n", rssBuf.getItemCount());
        }
        gState = AS_DISP;
        iItem = 0;
      } else {
        Serial.printf("[HTTP] GET ... failed, error: %s\n",
                      http.errorToString(httpCode).c_str());
        // delay(2000);
        scroll.printf("[[%sからの取得に失敗しました。]]  ", pSite->name);
        site_next();
        gState = AS_READ;
      }
      http.end();
    }
  } else if (gState == AS_DISP) {
    if (scroll.update()) {
      // delay(10);
    } else {
      if (iItem < rssBuf.getItemCount()) {
        scroll.print(rssBuf.getItem(iItem++));
        scroll.scrollOut(180);
      } else if (!sc_aux.update()) {
        site_next();
        gState = AS_READ;
      }
    }
  }
}

void rss_disp(LayoutElem *elem, bool bInit) {
  int16_t x = elem->x;
  int16_t y = elem->y;
  uint16_t w = elem->w;
  uint16_t h = elem->h;
  uint8_t textSize = h > 16 ? 2 : 1;

  if (bInit) {
    scroll.setSpeed(70 * textSize);
    scroll.setTextSize(textSize);
    scroll.setScrollArea(x, y, w, h);
    scroll.setPos();
  } else {
    rss_update();
  }
}

void rss_aux_disp(LayoutElem *elem, bool bInit) {
  static int iItem0;
  int16_t x = elem->x;
  int16_t y = elem->y;
  uint16_t w = elem->w;
  uint16_t h = elem->h;
  uint8_t textSize = h > 16 ? 2 : 1;
  static uint16_t tw;

  if (bInit) {
    int16_t x1, y1;
    uint16_t th;
    oled.getTextBounds("[00/00]", 0, 0, &x1, &y1, &tw, &th);

    iItem0 = -1;
    sc_aux.setSpeed(40);
    sc_aux.setTextSize(textSize);
    sc_aux.setFont(&fontx);
    sc_aux.setScrollArea(x, y, w - tw, h);
    sc_aux.setPos();
  } else {
    int total = rssBuf.getItemCount();
    if (!sc_aux.update()) {
      if (iItem < total) {
        sc_aux.print("  ↓  ");
        sc_aux.print(rssBuf.getTitle());
        if (iItem == total - 1) sc_aux.scrollOut();
      } else if (gState == AS_READ) {
        sc_aux.printf(" %s 取得中 ", pSite->name);
        sc_aux.scrollOut();
      }
    }
    if (iItem != iItem0) {
      oled.fillRect(x + w - tw, y, tw, h, 0);
      oled.setTextSize(textSize);
      oled.alignPrintf(x + w - 1, y + h - 1, TA_RIGHT, TA_BOTTOM, "[%d/%d]",
                       iItem, total);
      iItem0 = iItem;
    }
  }
}

void datetime_disp(LayoutElem *elem, bool bInit) {
  int16_t x = elem->x;
  int16_t y = elem->y;
  uint16_t w = elem->w;
  uint16_t h = elem->h;
  uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);

  if (bInit || t != t0) {
    struct tm *tm = localtime(&t);
    const char *wd[7] = {"日", "月", "火", "水", "木", "金", "土"};
    oled.fillRect(x, y, w, h, 0);
    oled.setTextColor(1, 0);
    oled.setTextSize(textSize);
    oled.alignPrintf(x + w - 1, y, TA_RIGHT, TA_TOP, "%d/%d(%s) %2d:%02d",
                     tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday], tm->tm_hour,
                     tm->tm_min);
    t0 = t;
  }
}

void date_disp(LayoutElem *elem, bool bInit) {
  int16_t x = elem->x;
  int16_t y = elem->y;
  uint16_t w = elem->w;
  uint16_t h = elem->h;
  uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);
  time_t dt = t / DAYSEC * DAYSEC;

  if (bInit || dt != t0) {
    struct tm *tm = localtime(&t);
    const char *wd[7] = {"日", "月", "火", "水", "木", "金", "土"};
    oled.setTextColor(1, 0);
    oled.setTextSize(textSize);
    oled.fillRect(x, y, w, h, 0);
    if (textSize == 1)
      oled.alignPrintf(x + w - 1, y, TA_RIGHT, TA_TOP, "%04d年%d月%d(%s)",
                       tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       wd[tm->tm_wday]);
    else if (textSize == 2)
      oled.alignPrintf(x + w - 1, y, TA_RIGHT, TA_TOP, "%d/%d %s",
                       tm->tm_mon + 1, tm->tm_mday, wd[tm->tm_wday]);

    t0 = dt;
  }
}

void time_disp(LayoutElem *elem, bool bInit) {
  int16_t x = elem->x;
  int16_t y = elem->y;
  uint16_t w = elem->w;
  uint16_t h = elem->h;
  uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);

  if (t != t0) {
    struct tm *tm = localtime(&t);
    oled.fillRect(x, y, w, h, 0);
    oled.setTextSize(textSize);
    oled.setTextColor(1, 0);
    oled.alignPrintf(x + w - 1, y, TA_RIGHT, TA_TOP, "%2d:%02d:%02d",
                     tm->tm_hour, tm->tm_min, tm->tm_sec);
    t0 = t;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\nReset:");

  oled.begin();
  oled.setRotation(0);
  oled.clear();
  oled.display();
  scroll.setTextSize(1);
  scroll.setPos();

  scroll.setFont(&fontx);
  oled.setFont(&fontx);
  oled.setTextWrap(false);

  oled.clear();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("connecting");
  oled.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    oled.print('.');
    oled.display();
    delay(500);
  }
  Serial.println();
  Serial.printf("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  oled.print("\nIP address: ");
  oled.println(WiFi.localIP());

  configTime(9 * 3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp", NULL);

  gState = AS_READ;

#if USE_OTA
  ota_init();
#endif

  delay(500);
  iLayout = 0;
  oled.clear();
  layout_set(&layouts[iLayout]);
  oled.display();
  tStart = millis();
}

void change_layout() {
  if (++iLayout >= sizeof(layouts) / sizeof(layouts[0])) iLayout = 0;
  scroll.clear();
  sc_aux.clear();
  oled.clear();
  layout_set(&layouts[iLayout]);
  oled.display();
}

void sw_update() {
  bool pushed = (digitalRead(SW_PIN) == LOW);

  switch (sw_state) {
    case SWS_Pushed:
      if (!pushed) {
        sw_release_time = millis();
        sw_state = SWS_Released;
      }
      break;

    case SWS_Released:
      if (pushed)
        sw_state = SWS_Pushed;
      else if (millis() - sw_release_time > 50) {
        sw_state = SWS_Stabled;
        change_layout();
      }
      break;

    case SWS_Stabled:
      if (pushed) sw_state = SWS_Pushed;
      break;
  }
}

void loop() {
#if USE_OTA
  ArduinoOTA.handle();
#endif
  layout_update(&layouts[iLayout]);
  sw_update();
}

/*** Local variables: ***/
/*** tab-width:2 ***/
/*** End: ***/
