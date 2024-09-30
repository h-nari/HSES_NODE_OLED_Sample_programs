#include "conf.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <time.h>
#include <FS.h>

#include <Adafruit_GFX.h>		// https://github.com/adafruit/Adafruit-GFX-Library
#include <Fontx.h>							// https://github.com/h-nari/Fontx
#include <Humblesoft_GFX.h>			// https://github.com/h-nari/Humblesoft_GFX
#include <Humblesoft_SSD1306.h>	// https://github.com/h-nari/Humblesoft_SSD1306
#include <fontx/ILGH16XB.h>
#include <fontx/ILGZ16XB.h>

#if USE_OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ota_util.h"
#endif

#define KANJI 1

#define OLED_DC		  15
#define OLED_RESET	16	
#define OLED_CS		   2

Humblesoft_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);
RomFontx fontx(ILGH16XB,ILGZ16XB);

void setup()
{
  Serial.begin(115200);
  oled.begin();
	
	oled.setRotation(2);
#if KANJI
	oled.setFont(&fontx);
#endif
	
  oled.clear();
	oled.display();
	
  delay(100);
  Serial.println("\n\nReset:");

	if(!SPIFFS.begin())
		Serial.println("SPIFFS failed.");
#if DUMP_SPIFFS
	else {
		Dir dir = SPIFFS.openDir("/");
		int cnt = 0;
		while(dir.next()){
			File f = dir.openFile("r");
			Serial.printf("[%d] %-12s %12u\n",++cnt,f.name(), f.size());
			f.close();
		}
		Serial.printf("%d files found.\n",cnt);
	}
#endif
  
	oled.println("Reset:");
	WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		oled.print('.');
		oled.display();
	}
	oled.println();
	oled.print("IP: ");
	oled.println(WiFi.localIP());
	Serial.print("IP: ");
	Serial.println(WiFi.localIP());

	if(MDNS.begin("esp8266"))
		oled.println("MDNS started");
  configTime( 9 * 3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
#if USE_OTA
	ota_init();
#endif
}

void loop()
{
	static time_t t0;
	time_t t = time(NULL);
	struct tm *tm;
	
	if(t != t0){
#if KANJI
		static const char *wd[7] = {"日","月","火","水","木","金","土"};
#else
		static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
#endif
		tm = localtime(&t);

		oled.clear();
		oled.setCursor(0,0);
		oled.setTextWrap(false);
		
#if KANJI
		oled.setTextSize(1);
		oled.printf(" %4d / 令和%d年\n", tm->tm_year+1900, tm->tm_year-118);
		oled.printf("%2d月%2d日  %s曜日\n",
								tm->tm_mon+1, tm->tm_mday, wd[tm->tm_wday]);
		oled.setTextSize(2);
		oled.printf("%02d:%02d:%02d",tm->tm_hour, tm->tm_min, tm->tm_sec);
#else // not KANJI
		oled.setTextSize(2);
		oled.printf("%4d\n",tm->tm_year+1900);
		oled.setCursor(0, oled.getCursorY()+8);
		oled.printf("%d/%d(%s)\n",tm->tm_mon+1, tm->tm_mday, wd[tm->tm_wday]);
		oled.setCursor(0, oled.getCursorY()+8);
		oled.printf("%02d:%02d:%02d",tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
		oled.display();
		t0 = t;
	}
	
#if USE_OTA
	ArduinoOTA.handle();
#endif
}

/*** Local variables: ***/
/*** tab-width:2 ***/
/*** End: ***/
