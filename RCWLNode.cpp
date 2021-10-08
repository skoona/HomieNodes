/**
 * Homie Node for RCWL-0516 Doppler Radar Microwave Motion Sensor
 * 
 */
#include "RCWLNode.hpp"

RCWLNode::RCWLNode(const char *id, const char *name, const char *nType, const uint8_t rcwlPin, const int motionHoldInterval = HOLD_INTERVAL)
    : HomieNode(id, name, nType, false, 0U, 0U)
{
  _motionHoldInterval = (motionHoldInterval > MIN_INTERVAL) ? motionHoldInterval : MIN_INTERVAL;
  _motionPin = rcwlPin;
  _motion = false;

  // Start up the library
  pinMode(_motionPin, INPUT); 
}

/**
 *
 */
void RCWLNode::onReadyToOperate() { 
    Homie.getLogger() << cCaption << endl;
    Homie.getLogger() << cIndent << "onReadyToOperate()" << endl;
}

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void RCWLNode::setup() {
  Homie.getLogger() << cCaption << endl;
  Homie.getLogger() << cIndent << cPropertyName << endl;

  advertise(cProperty)
      .setName(cPropertyName)
      .setDatatype(cPropertyDataType)
      .setFormat(cPropertyFormat);
      // .setUnit(cPropertyUnit);
}

/**
 * Called by Homie when homie is connected and in run mode
*/
void RCWLNode::loop() {
  _isrTrigger = digitalRead(_motionPin);

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

