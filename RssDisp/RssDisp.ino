#include "conf.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

#if USE_OTA
#include <ArduinoOTA.h>
#include "ota_util.h"
#endif

#include <Adafruit_GFX.h>		
#include <Fontx.h>							
#include <fontx/ILGH16XB.h>
#include <fontx/ILGZ16XB.h>
#include <Humblesoft_GFX.h>			
#include <Humblesoft_SSD1306.h>	
#include <ScrollText.h>
#include "rssParser.h"
#include "rssBuf.h"
#include "layout.h"

#define DAYSEC  (3600*24)

#define SW_PIN			 0
#define OLED_DC			15
#define OLED_RESET	16	
#define OLED_CS		 	 2

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

const char *site[] = {
	"http://news.yahoo.co.jp/pickup/rss.xml",
  "http://numbers2007.blog123.fc2.com/?xml",
  "http://feeds.feedburner.com/make_jp",
  "http://www.jiji.com/rss/ranking.rdf",
  "http://rss.rssad.jp/rss/impresswatch/pcwatch.rdf",
	"https://headlines.yahoo.co.jp/rss/afpbbnewsv-c_int.xml",
  "http://headlines.yahoo.co.jp/rss/natiogeog-c_int.xml",
  "http://rss.asahi.com/rss/asahi/newsheadlines.rdf",
  NULL};


Layout layouts[] = {
	{
		{
			{ 0,  0, 128, 16, datetime_disp},
			{ 0, 16, 128, 16, rss_aux_disp },
			{ 0, 32, 128, 32, rss_disp },
		},
	},
	{
		{
			{ 0,  0, 128, 16, date_disp},
			{ 0, 16, 128, 16, time_disp},
			{ 0, 32, 128, 16, rss_aux_disp },
			{ 0, 48, 128, 16, rss_disp },
		},
	},
	{
		{
			{ 0,  0, 128, 16, date_disp},
			{ 0, 16, 128, 16, time_disp},
			{ 0, 32, 128, 32, rss_disp },
		},
	},
	{
		{
			{ 0,  0, 128, 16, date_disp},
			{ 0, 16, 128, 32, time_disp},
			{ 0, 48, 128, 16, rss_disp },
		},
	},
	{
		{
			{ 0,  0, 128, 32, date_disp},
			{ 0, 32, 128, 16, time_disp},
			{ 0, 48, 128, 16, rss_disp },
		},
	}
};

AppState gState;
int httpCode;
int iItem;
int iLayout;

RssBuf rssBuf;
RssParser rss;
const char **pSite = site;

SW_STATE sw_state = SWS_Stabled;
unsigned long sw_release_time;

RomFontx fontx(ILGH16XB,ILGZ16XB);
Humblesoft_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);
ScrollText scroll(&oled);
ScrollText sc_aux(&oled);


void rss_update()
{
  if(gState == AS_READ){
    rssBuf.clear();
    
    HTTPClient http;
    Serial.printf("[HTTP]begin..\n");
    
    http.begin(*pSite);
    Serial.printf("[HTTP]GET %s\n",*pSite);

    httpCode = http.GET();
    if(httpCode > 0){
      Serial.printf("[HTTP] GET ... code: %d\n",httpCode);
      if(httpCode == HTTP_CODE_OK) {
				int len = http.getSize();
				uint8_t buff[128] = {0};
				WiFiClient *stream = http.getStreamPtr();
				while(http.connected() && (len > 0 || len == -1)){
					size_t size = stream->available();
					if(size){
						if(size > sizeof buff) size = sizeof buff;
						int c = stream->readBytes(buff,size);
						for(int i=0; i<c; i++)
							rss.put(buff[i]);
						if(len > 0) len -= c;
					}
					delay(0);
				}
				Serial.printf("[HTTP] ItemCount:%d\n", rssBuf.getItemCount());
      }
      gState = AS_DISP;
			iItem = 0;
    } else {
      Serial.printf("[HTTP] GET ... failed, error: %s\n",
										http.errorToString(httpCode).c_str());
      if(*++pSite == NULL) pSite = site;
      gState = AS_READ;
    }
    http.end();
  }
  else if(gState == AS_DISP) {
		if(scroll.update()){
			// delay(10);
		} else {
			if(iItem < rssBuf.getItemCount()){
				scroll.print(rssBuf.getItem(iItem++));
				scroll.scrollOut();
			} else {
				if(*++pSite == NULL) pSite = site;
				gState = AS_READ;
			}
		}
  }
}

void rss_disp(LayoutElem *elem, bool bInit)
{
	int16_t  x = elem->x;
	int16_t  y = elem->y;
	uint16_t w = elem->w;
	uint16_t h = elem->h;
	uint8_t textSize = h > 16 ? 2 : 1;
	
	if(bInit){
		scroll.setSpeed(70 * textSize);
		scroll.setTextSize(textSize);
		scroll.setScrollArea(x, y, w, h);
		scroll.setPos();
	} else {
		rss_update();
	}
}

void rss_aux_disp(LayoutElem *elem, bool bInit)
{
	static int iItem0;
	int16_t  x = elem->x;
	int16_t  y = elem->y;
	uint16_t w = elem->w;
	uint16_t h = elem->h;
	uint8_t textSize = h > 16 ? 2 : 1;
	static uint16_t tw;
	
	if(bInit){
		int16_t  x1,y1;
		uint16_t th;
		oled.getTextBounds("[00/00]", 0, 0, &x1, &y1, &tw, &th);

		iItem0 = -1;
		sc_aux.setSpeed(40);
		sc_aux.setTextSize(textSize);
		sc_aux.setFont(&fontx);
		sc_aux.setScrollArea(x, y, w-tw, h);
		sc_aux.setPos();
	} else {
		if(!sc_aux.update()){
			sc_aux.print("  ↓  ");
			sc_aux.print(rssBuf.getTitle());
		}
		if(iItem != iItem0) {
			oled.fillRect(x+w-tw,y,tw,h,0);
			oled.setTextSize(textSize);
			oled.alignPrintf(x+w-1,y+h-1, TA_RIGHT, TA_BOTTOM, "[%d/%d]",
											 iItem,rssBuf.getItemCount());
			iItem0 = iItem;
		}
	}
}

void datetime_disp(LayoutElem *elem, bool bInit)
{
	int16_t  x = elem->x;
	int16_t  y = elem->y;
	uint16_t w = elem->w;
	uint16_t h = elem->h;
	uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);

  if(bInit || t != t0){
    struct tm *tm = localtime(&t);
    const char *wd[7] = {"日","月","火","水","木","金","土"};
		oled.setTextColor(1,0);
    oled.setTextSize(textSize);
		oled.alignPrintf(x+w-1,y, TA_RIGHT, TA_TOP,
										 "%2d/%2d(%s) %d:%d", 
										 tm->tm_mon+1, tm->tm_mday, wd[tm->tm_wday],
										 tm->tm_hour, tm->tm_min);
    t0 = t;
  }
}

void date_disp(LayoutElem *elem, bool bInit)
{
	int16_t  x = elem->x;
	int16_t  y = elem->y;
	uint16_t w = elem->w;
	uint16_t h = elem->h;
	uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);
	time_t dt = t / DAYSEC * DAYSEC;

  if(bInit || dt != t0){
    struct tm *tm = localtime(&t);
    const char *wd[7] = {"日","月","火","水","木","金","土"};
		oled.setTextColor(1,0);
    oled.setTextSize(textSize);
		if(textSize == 1)
			oled.alignPrintf(x+w-1,y, TA_RIGHT, TA_TOP,
											 "%04d年%0d月%2d(%s)", tm->tm_year + 1900,
											 tm->tm_mon+1, tm->tm_mday, wd[tm->tm_wday]);
		else if(textSize == 2)
			oled.alignPrintf(x+w-1,y, TA_RIGHT, TA_TOP,
											 "%d/%d %s", tm->tm_mon+1, tm->tm_mday, wd[tm->tm_wday]);
			
    t0 = dt;
  }
}

void time_disp(LayoutElem *elem, bool bInit)
{
	int16_t  x = elem->x;
	int16_t  y = elem->y;
	uint16_t w = elem->w;
	uint16_t h = elem->h;
	uint8_t textSize = h > 16 ? 2 : 1;
  static time_t t0;
  time_t t = time(NULL);

  if(t != t0){
    struct tm *tm = localtime(&t);
    oled.setTextSize(textSize);
		oled.setTextColor(1,0);
    oled.alignPrintf(x+w-1,y, TA_RIGHT, TA_TOP,
										 "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
    t0 = t;
  }
}

void setup()
{
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
	oled.setCursor(0,0);
	oled.println("connecting to network"); 
	oled.display();
		
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while(WiFi.status() != WL_CONNECTED) {
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

	configTime( 9 * 3600, 0,
							"ntp.nict.jp", "ntp.jst.mfeed.ad.jp", NULL);
    
	gState = AS_READ;
	
#if USE_OTA
  ota_init();
#endif

	delay(500);
	iLayout = 0;
	oled.clear();
	layout_set(&layouts[iLayout]);
	oled.display();
}

void change_layout()
{
	if(++iLayout >= sizeof(layouts)/sizeof(layouts[0]))
		iLayout = 0;
	scroll.clear();
	sc_aux.clear();
	oled.clear();
	layout_set(&layouts[iLayout]);
	oled.display();
}

void sw_update()
{
	bool pushed = (digitalRead(SW_PIN) == LOW); 

	switch(sw_state){
	case SWS_Pushed:
		if(!pushed){
			sw_release_time = millis();
			sw_state = SWS_Released;
		}
		break;
			
	case SWS_Released:
		if(pushed)
			sw_state = SWS_Pushed;
		else if(millis() - sw_release_time > 50){
			sw_state = SWS_Stabled;
			change_layout();
		}
		break;
		
	case SWS_Stabled:
		if(pushed)
			sw_state = SWS_Pushed;
		break;
	}
}

void loop()
{
#if USE_OTA
  ArduinoOTA.handle();
#endif
	layout_update(&layouts[iLayout]);
	sw_update();
}

/*** Local variables: ***/
/*** tab-width:2 ***/
/*** End: ***/
