#include "conf.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <time.h>

#include <Adafruit_GFX.h>		// https://github.com/adafruit/Adafruit-GFX-Library
#include <Fontx.h>							// https://github.com/h-nari/Fontx
#include <Humblesoft_GFX.h>			// https://github.com/h-nari/Humblesoft_GFX
#include <Humblesoft_SSD1306.h>	// https://github.com/h-nari/Humblesoft_SSD1306
#include <fonts/FreeMono9pt7b.h>

#if USE_OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ota_util.h"
#endif

#define OLED_DC		  15
#define OLED_RESET	16	
#define OLED_CS		   2

Humblesoft_SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);

void setup()
{
  Serial.begin(115200);
  oled.begin();
	oled.setRotation(2);
	oled.setTextWrap(false);
	
  oled.clear();
	oled.display();
	
  delay(100);
  Serial.println("\n\nReset:");
  
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
		tm = localtime(&t);

		oled.clear();

		int16_t fg = Humblesoft_SSD1306::WHITE;
		int16_t bg = Humblesoft_SSD1306::BLACK;
		int16_t xc = 3 * SSD1306_WIDTH / 4;
		int16_t yc = SSD1306_HEIGHT / 2;

		int16_t r_frame = 31;
		int16_t r_tic1  = 27;
		int16_t r_tic2  = 26;
		int16_t r_hour  = 15;
		int16_t r_min   = 25;
		int16_t r_sec   = 20;

		// frame

		oled.drawCircle(xc, yc, 30, fg);
		for(int h=0; h<12; h++){
			double at = M_PI * 2.0 * h / 12.0;
			int16_t x1 = xc + r_tic1 * sin(at);
			int16_t x2 = xc + r_tic2 * sin(at);
			int16_t y1 = yc - r_tic1 * cos(at);
			int16_t y2 = yc - r_tic2 * cos(at);
			oled.drawLine(x1,y1,x2,y2,fg);
		}
		
		// hour

		double ah = M_PI * 2.0 * ((tm->tm_hour % 12) + tm->tm_min / 60.0) /12.0;
		int16_t xh = xc + r_hour * sin(ah);
		int16_t yh = yc - r_hour * cos(ah);
		oled.drawLine(xc, yc, xh, yh, fg);
		
		// min

		double am = M_PI * 2.0 * tm->tm_min / 60.0;
		int16_t xm = xc + r_min * sin(am);
		int16_t ym = yc - r_min * cos(am);
		oled.drawLine(xc, yc, xm, ym, fg);
		
		// sec
		
		double as = M_PI * 2.0 * tm->tm_sec / 60.0;
		int16_t xs = xc + r_sec * sin(as);
		int16_t ys = yc - r_sec * cos(as);
		oled.fillCircle(xs, ys, 2, bg);
		oled.drawCircle(xs, ys, 2, fg);

		// text

		static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};

		oled.setFont(&FreeMono9pt7b);
		oled.setCursor(0,0);
		oled.printf("\n%d/%d\n %s\n",tm->tm_mon+1,tm->tm_mday, wd[tm->tm_wday]);
		oled.printf("%2d:%02d", tm->tm_hour,tm->tm_min);

		// display
		
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
