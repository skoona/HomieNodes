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

  bool isBeamBroken();
  double getBeamVoltage();

protected:
  virtual void setup() override;
  virtual void loop() override;
  virtual void onReadyToOperate() override;
  
private:
  int _pinADC;
  char buffer[32];

  const char *cCaption = "• ESP32 ADC Measurements";
  const char* cIndent  = "  ◦ ";

  // ADC Node Properties
  const char *cPropertyStatus = "beamstatus";
  const char *cPropertyStatusName = "Safety Beam Status";
  const char *cPropertyStatusDataType = "enum";
  const char *cPropertyStatusFormat = "OPEN,CLOSED";
  const char *cPropertyStatusUnit = "#";

  const char *cPropertyVoltage = "beamvoltage";
  const char *cPropertyVoltageName = "Saftey Beam Volts";
  const char *cPropertyVoltageDataType = "float";
  const char *cPropertyVoltageFormat = "%03.1f";
  const char *cPropertyVoltageUnit = "vdc";

  volatile bool  _adcStatus = false; // open(broken=true), close
  volatile double _adcVoltage = 0.0;
  volatile double _threashold = 19.5;

  bool bTriggered = false;

  unsigned long ulTriggerBase = 0,     // baseline
                ulLastTriggerBase = 0, // increase 
                ulTriggerHold = 20000, // ms = 20 secs
                ulTriggerCycle = 500,  // sample rate
                ulTriggerDuration = 0, // hold time once triggered
                ulElapsed = 0;
  
  void initializeADC();
  double sknAdcToVolts(int value);
  double readADC();
}
