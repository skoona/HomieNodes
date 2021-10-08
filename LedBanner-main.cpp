// ESP8266 DevKit V1 source
#include <Arduino.h>

#include <Homie.h>
#include "LedBanner.hpp"

#define SKN_MOD_NAME "Max7219 Banner"
#define SKN_MOD_VERSION "1.0.4"
#define SKN_MOD_BRAND "SknSensors"

#define SKN_NODE_TITLE "Message Banner"
#define SKN_NODE_TYPE "8x8x4 LED"
#define SKN_NODE_ID "LEDBanner"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

// SPI Pins
#ifdef ESP8266
#define CLK_PIN  14  // D5  // or SCK
#define DATA_PIN 13  // D7  // or MOSI
#define CS_PIN   15  // D8  // or SS
#else
#define CLK_PIN  18  // or SCK
#define DATA_PIN 23  // or MOSI
#define CS_PIN    5  // or SS
#endif

HomieSetting<long> cfgBrightness("brightness", "The intensity of the leds from 0 thru 15.");
HomieSetting<long> cfgSpeed("speed", "The scrolling speed in milliseconds.");

LedBanner banner(SKN_NODE_ID, SKN_NODE_TITLE, SKN_NODE_TYPE, HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

bool broadcastHandler(const String &level, const String &value)
{
  Homie.getLogger() << "Received broadcast level " << level << ": " << value << endl;
  char newMessage[255];

  snprintf(newMessage, sizeof(newMessage) - 3, "%s: %s", level.c_str(), value.c_str());
  banner.setNewMessage(newMessage);

  return true;
}

void setup()
{
  Serial.begin(115200);

  Homie_setFirmware(SKN_MOD_NAME, SKN_MOD_VERSION);
  Homie_setBrand(SKN_MOD_BRAND);

  cfgBrightness
      .setDefaultValue(6)
      .setValidator([](long candidate)
                    { return candidate > 0 && candidate < 15; });

  cfgSpeed
      .setDefaultValue(60)
      .setValidator([](long candidate)
                    { return candidate > 0 && candidate < 150; });

  banner.setLedBrightness((uint8_t)cfgBrightness.get());
  banner.setLedScrollSpeed((uint8_t)cfgSpeed.get());

  Homie.setBroadcastHandler(broadcastHandler)
      .setLedPin(LED_BUILTIN, LOW)
      .disableResetTrigger();

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
