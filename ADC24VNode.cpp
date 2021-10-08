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

/**
 * true is broken
 * false is closed
 */
bool ADC24VNode::isBeamBroken()
{
  return _adcStatus;
}

/**
 * actual
 */
double ADC24VNode::getBeamVoltage()
{
  return _adcVoltage;
}

/*
 * Utility to handle Duration Roll Overs
*/
unsigned long setDuration(unsigned long duration)
{
  unsigned long value = millis() + duration;
  if (value < duration)
  { // rolled
    value = duration;
  }
  return value;
}

/**
 * - Value ranges from 18.5 to 20.5
*/
double ADC24VNode::sknAdcToVolts(int value)
{
  if (value < 1 || value > 4095)
  {
    return 1.0;
  }

  double reading = value * 1.0; // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095

  reading = -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
  return ((reading + ENTRY_VOLTAGE_BASE) * 10.0); // scale to d.dd
}

/**
 *
 */
double ADC24VNode::readADC()
{
  _adcVoltage = sknAdcToVolts( adc1_get_raw(ADC1_CHANNEL_0) );
  return _adcVoltage;
}

/**
 *
 */
void ADC24VNode::onReadyToOperate() { 
    Homie.getLogger() << cCaption << endl;
    Homie.getLogger() << cIndent << getName() << " onReadyToOperate(): volts=" << readADC() << endl;
}

/**
 * Called by Homie during Homie.setup() is called; Once!
 * 
 * Voltage divider analog in pins
 * https://dl.espressif.com/doc/esp-idf/latest/api-reference/peripherals/adc.html
 * set up A:D channels and attenuation
 *   * Read the sensor by
 *   * uint16_t value =  adc1_get_raw(ADC1_CHANNEL_0);
 *  150mv-3.9vdc inside 0-4095 range   (constrained by 3.3vdc supply)
*/
void ADC24VNode::setup()
{
  pinMode(_pinADC, INPUT);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Pin 36

  readADC(); // side effect of setting _adcVoltage

  advertise(cPropertyStatus)
    .setName(cPropertyStatusName)
    .setDatatype(cPropertyStatusDataType)
    .setFormat(cPropertyStatusFormat)
    .setUnit(cPropertyStatusUnit);

  advertise(cPropertyVoltage)
    .setName(cPropertyVoltageName)
    .setDatatype(cPropertyVoltageDataType)
    .setFormat(cPropertyVoltageFormat)
    .setUnit(cPropertyVoltageUnit);

  ulTriggerbase = millis();
}

/**
 * Called by Homie when homie is connected and in run mode
*/
void ADC24VNode::loop() {
  ulTriggerBase = millis();
  ulElapsed = ulTriggerBase - ulLastTriggerBase;

  if (ulElapsed >= ulTriggerCycle) // sample
  {
    readADC();

    Homie.getLogger() << cIndent 
                      << "✖ SafetyBeam: " << getName()
                      << " Volts: " << _adcVoltage
                      << endl;

    if (_adcVoltage >= _threashold)
    {
      if (!bTriggered) 
      { // ON
        snprintf(buffer, sizeof(buffer), cPropertyVoltageFormat, _adcVoltage);
        setProperty(cPropertyStatus).send("OPEN");
        setProperty(cPropertyVoltage).send(buffer);
      }
      ulTriggerDuration = setDuration(ulTriggerHold);
      bTriggered = true;  
    }

    ulLastTriggerBase = ulTriggerBase;
  }

  if (bTriggered && (ulTriggerBase >= ulTriggerDuration))
  { // OFF                     
    bTriggered = false;

    snprintf(buffer, sizeof(buffer), cPropertyVoltageFormat, _adcVoltage);
    setProperty(cPropertyStatus).send("CLOSED");
    setProperty(cPropertyVoltage).send(buffer);
  }
}

