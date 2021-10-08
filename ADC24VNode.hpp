/**
 * Homie Node for an ESP32 ADC interface
 * 
 * The target device being monitored is an Garage Door Safety Beam, which when broken
 * causes the garage door to OPEN if it were in the process of closing.
 *
 * It is a 24VDC configuration that reads 20.1vdc when beam is broken, 
 * and 18.5vdc otherwise.
 * 
 * GPIO 36 is used as the ADC Input with an external voltage divider circuit.
 * 
 * A custom adc smoothing algorithm is used to improve the stability 
 * and accuracy of the reading.  
 * 
 * The door typically travels 8ft or 2500mm and takes 40 seconds to operate.
 * 
 */

#pragma once

#include <driver/adc.h>
#include <Homie.hpp>

#ifndef ESP32
#pragma message(THIS MODULE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif

class ADC24VNode : public HomieNode {

public:
  ADC24VNode(const char *id, const char *name, const char *nType, const uint8_t adcPin);

  void setMotionHoldInterval(unsigned long interval) { _motionHoldInterval = interval; }
  unsigned long getMotionHoldInterval() const { return _motionHoldInterval; }

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;
  
private:
  // suggested rate is 1/60Hz (1m)
  static const int MIN_INTERVAL  = 10;  // in seconds
  static const int HOLD_INTERVAL = 60;

  const char *cCaption = "• RCWL-0516 Doppler Radar Microwave Motion Sensor:";
  const char* cIndent  = "  ◦ ";

  // Motion Node Properties
  int _pinADC;
  const char *cPropertyStatus = "beamstatus";
  const char *cPropertyStatusName = "Safety Beam Status";
  const char *cPropertyStatusDataType = "enum";
  const char *cPropertyStatusFormat = "OPEN,CLOSED";
  const char *cPropertyStatusUnit = "#";

  const char *cPropertyVoltage = "safetybeam";
  const char *cPropertyVoltageName = "Saftey Beam Volts";
  const char *cPropertyVoltageDataType = "float";
  const char *cPropertyVoltageFormat = "%03.1f";
  const char *cPropertyVoltageUnit = "vdc";

  volatile bool  _adcStatus = false;
  volatile float _adcVoltage = 0.0;
  volatile float _threashold = 19.5;
  
  void initializeADC();
};
