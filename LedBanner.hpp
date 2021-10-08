/**
 * Homie Node for Max7219 8x8 Banners.
 *
 */

#pragma once

#include <Homie.hpp>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define SKN_NODE_MESSAGE_PROPERTY_NAME "Banner Text"
#define SKN_NODE_MESSAGE_PROPERTY_ID "message"

#define SKN_NODE_SPEED_PROPERTY_NAME "Scrolling Speed"
#define SKN_NODE_SPEED_PROPERTY_ID "speed"

#define SKN_NODE_BRIGHTNESS_PROPERTY_NAME "Display Brightness"
#define SKN_NODE_BRIGHTNESS_PROPERTY_ID "brightness"

class LedBanner : public HomieNode
{

public:
    LedBanner(const char *id, const char *name, const char *cType, const MD_MAX72XX::moduleType_t ledHwType, const int dataPin, const int clkPin, const int csPin, const int maxDevices);

    void setNewMessage(const char *pMsg);
    void setLedBrightness(uint8_t value);
    void setLedScrollSpeed(uint8_t value);

protected:
    virtual void setup() override;
    virtual void onReadyToOperate() override;
    virtual bool handleInput(const HomieRange &range, const String &property, const String &value);
    virtual void loop() override;

private:
    int _dataIn;
    int _clock;
    int _chipSelect;
    int _devices;
    MD_MAX72XX::moduleType_t _ledHardwareType;

    uint8_t brightness  = 6;
    uint8_t scrollSpeed = 50; // default frame delay value  85 too slow, 25 too fast
    bool newSpeedAvailable = false;
    bool newBrightnessAvailable = false;

    textEffect_t scrollEffect = PA_SCROLL_LEFT;
    textPosition_t scrollAlign = PA_LEFT;
    uint16_t scrollPause = 2000; // in milliseconds

    static const uint16_t MESG_SIZE = 512;
    char curMessage[MESG_SIZE];
    char newMessage[MESG_SIZE];
    bool newMessageAvailable = false;

    // SPI hardware interface
    MD_Parola Pmx = MD_Parola(_ledHardwareType, _dataIn, _clock, _chipSelect, _devices);

    void printCaption();
    void displaySetupHandler();
};