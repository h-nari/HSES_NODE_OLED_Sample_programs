#include "conf.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#if USE_OTA
#include <ArduinoOTA.h>
#include "ota_util.h"
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Fontx.h>
#include <Humblesoft_GFX.h>
#include <Humblesoft_SSD1306.h>

WiFiUDP udp;

struct PacketHeader {
  char sig[8];
  uint32_t frame;
};

Humblesoft_SSD1306 oled;

void setup()
{
  Serial.begin(115200);
  oled.begin();
  delay(100);
  Serial.println("\n\nReset:");
  oled.println("reset");
  oled.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
    oled.print('.');
    oled.display();
  }
  Serial.println();
  oled.println();
  
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  oled.println(WiFi.localIP());
  oled.display();
  
  udp.begin(9012);

#if USE_OTA
  ota_init();
#endif
}

void loop()
{
#if USE_OTA
  ArduinoOTA.handle();
#endif
  
  if(udp.parsePacket()){
    PacketHeader h;
    int len = udp.read((uint8_t *)&h, sizeof(h));

    if(len < sizeof(h)){
      Serial.printf("Bad length:%d\n",len);
    } else {
      while(udp.available()){
	uint8_t buf[1024];
	int len = udp.read(buf, sizeof(buf));
	oled.writeData(buf, len);
      }
    }
  }
}
