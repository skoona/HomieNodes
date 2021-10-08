/**
 * Homie Node for an ESP32 ADC interface
 * 
 */
#include "ADC24VNode.hpp"

ADC24VNode::ADC24VNode(const char *id, const char *name, const char *nType, const uint8_t adcPin)
    : HomieNode(id, name, nType, false, 0U, 0U),
      _pinADC = adcPin
{
}

/*
  * Voltage divider analog in pins
  * https://dl.espressif.com/doc/esp-idf/latest/api-reference/peripherals/adc.html
  * set up A:D channels and attenuation
  *   * Read the sensor by
  *   * uint16_t value =  adc1_get_raw(ADC1_CHANNEL_0);
  *  150mv-3.9vdc inside 0-4095 range   (constrained by 3.3vdc supply)
*/
void ADC24VNode::initializeADC()
{
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Pin 36
  taskYIELD();
}

/**
 *
 */
void ADC24VNode::onReadyToOperate() { 
    Homie.getLogger() << cCaption << endl;
    Homie.getLogger() << cIndent << "onReadyToOperate()" << endl;
}

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void ADC24VNode::setup() {
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << cPropertyName << endl;

  pinMode(_pinADC, INPUT);

  advertise(cProperty)
      .setName(cPropertyName)
      .setDatatype(cPropertyDataType)
      .setFormat(cPropertyFormat);
      // .setUnit(cPropertyUnit);
  advertise(cProperty)
      .setName(cPropertyName)
      .setDatatype(cPropertyDataType)
      .setFormat(cPropertyFormat);
  // .setUnit(cPropertyUnit);
}

/**
 * Called by Homie when homie is connected and in run mode
*/
void ADC24VNode::loop() {
  _isrTrigger = digitalRead(_pinADC);

  if (_isrTrigger == HIGH) {
    _isrTriggeredAt = millis(); // push hold time

    if (!_motion)
    {
      _motion = true;

      Homie.getLogger() << F("〽 Sending Presence: ") << endl;

      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: ON ")
                        << endl;

      setProperty(cProperty).setRetained(true).send("ON");
    }
  }

  if (_isrTriggeredAt != 0 ) {
    if (_motion && ((millis() - _isrTriggeredAt) >= (_motionHoldInterval * 1000UL))) {
      // hold time expired

      _motion = false;          // re-enable motion
      _isrTriggeredAt = 0;

      Homie.getLogger() << cIndent
                        << F("✖ Motion Detected: OFF ")
                        << endl;

      setProperty(cProperty).setRetained(true).send("OFF");
    }
  }
}

