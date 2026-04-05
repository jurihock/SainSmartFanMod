// ATtiny85:  ArduinoISP + ATTinyCore
// Clock:     8 MHz (internal)
// PWM:       25 kHz on PB4 (D4, OC1B) using Timer1 with PLL clock and /16 prescaler
// FAN:       PB3 (D3), MOSFET gate
// POT:       A1 (PB2)

const uint8_t FAN = 3;    // PB3 -> MOSFET gate (HIGH = ON)
const uint8_t PWM = 4;    // PB4 -> OC1B -> NPN base (sinks fan PWM, active-low at fan)
const uint8_t LED = 1;    // PB1
const uint8_t POT = A1;   // PB2
const uint8_t TOP = 159;  // Timer1 TOP for 25 kHz: f = (PLL 64MHz / 16) / (TOP+1) = 4MHz / 160 = 25kHz
const float   MIN = 0.2f; // min. duty cycle 20%

float getDuty()
{
  float duty = analogRead(POT) / 1023.0f;
  
  duty = powf(duty, 1.5f); // apply gamma
  duty = duty * (1.0f - MIN) + MIN; // scale to 20%..100%

  return duty;
}

void setDuty(float duty)
{
  duty = constrain(duty, 0.0f, 1.0f);
  duty = 1.0f - duty; // active-low at fan (NPN sink)

  uint16_t ticks = (uint16_t)((TOP + 1) * duty);

  if (ticks > 0)
  {
    ticks -= 1;
  }

  OCR1B = (uint8_t)ticks;
}

void setFan(bool state)
{
  if (state)
  {
    digitalWrite(LED, LOW);  // LED OFF
    pinMode(PWM, OUTPUT);
    GTCCR |= _BV(COM1B1);    // enable OC1B on pin (non-inverting)
    digitalWrite(FAN, HIGH); // MOSFET ON
    delay(1000);
  }
  else
  {
    digitalWrite(LED, HIGH); // LED ON
    digitalWrite(FAN, LOW);  // MOSFET OFF
    delay(1000);             // let it settle
    GTCCR &= ~_BV(COM1B1);   // disconnect OC1B from pin
    pinMode(PWM, INPUT);     // Hi-Z to avoid backfeed
  }
}

void setup()
{
  pinMode(FAN, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(LED, OUTPUT);

  // Enable PLL and route to Timer1
  PLLCSR = _BV(PLLE);                 // enable PLL
  delay(2);
  while (!(PLLCSR & _BV(PLOCK))) {}   // wait for lock
  PLLCSR |= _BV(PCKE);                // PLL clock to Timer1

  // Timer1 setup: Fast PWM, TOP = OCR1C, non-inverting on OC1B, prescaler = /16
  TCCR1 = 0;
  GTCCR = 0;

  // Prescaler /16 and PWM enable
  TCCR1 = _BV(PWM1A) | _BV(CS12) | _BV(CS10);  // CS1x = 0b0101 -> /16
  GTCCR = _BV(PWM1B) | _BV(COM1B1);            // enable OC1B, non-inverting

  OCR1C = TOP;     // sets 25 kHz
  OCR1B = 0;       // start at 0% (line high at fan)

  setFan(false);
}

void loop()
{
  static float oldDuty = MIN;
  static bool oldState = false;

  float newDuty = (getDuty() + oldDuty) / 2.0f;
  bool newState = oldState;

  // schmitt trigger around MIN
  float newDelta = newDuty - MIN;

  if (!newState && newDelta >= 0.03f)
  {
    // turn ON when duty ≥ MIN + 3%
    newState = true;
  }
  else if (newState && newDelta <= 0.01f)
  {
    // turn OFF when duty ≤ MIN + 1%
    newState = false;
  }

  if (newDuty != oldDuty)
  {
    setDuty(newDuty);
  }

  if (newState != oldState)
  {
    setFan(newState);
  }

  oldDuty = newDuty;
  oldState = newState;

  delay(100);
}
