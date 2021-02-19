#include <Arduino.h>

// ########################## DEFINES ##########################

#define TRIAC 6
//#define ANALOG_IN A0
#define ThermistorPin A6
#define DEBUG 0
#define TEMP 1
#define POT 0
#define DELAY0 10          //On state Triac delay
#define AC_FREQUENCY 10000 // 10000 for 50hz, 8333 for 60hz
#define TEMP_MIN 35        // Température de démarrage du Ventilateur en °C
#define TEMP_MAX 60        // Température de vitesse Maximum du ventilateur en °C

// ########################## INT ##########################
int speed_val = 0;
uint8_t start = 0;

//########################## TEMP ##########################
#if TEMP
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
#endif

int fonction123()
{
#if TEMP
  speed_val = map(Tc, TEMP_MIN, TEMP_MAX, 35, 1); // varie de 13 a 30
#endif

#if POT
  speed_val = map(analogRead(A0), 0, 1023, 1, 35); // varie de 13 a 30
#endif
  if (speed_val > 30)
  {
    start = 0;
  }
  if (speed_val > 1 && speed_val < 30)
  {
    start = 1;
  }
  if (speed_val <= 1)
  {
    start = 2;
  }

  return (speed_val);
}

void zero_crossing()
{
  fonction123();

  if (start == 0)
  {
    digitalWrite(TRIAC, LOW);
  }

  if (start == 1)
  {
    int chop_time = (200 * speed_val);
    delayMicroseconds(chop_time);
    digitalWrite(TRIAC, HIGH);
    delayMicroseconds(DELAY0);
    digitalWrite(TRIAC, LOW);
    delayMicroseconds(AC_FREQUENCY - DELAY0);
    digitalWrite(TRIAC, HIGH);
    delayMicroseconds(DELAY0);
    digitalWrite(TRIAC, LOW);
  }

  if (start == 2)
  {
    digitalWrite(TRIAC, HIGH);
  }
}

void setup()
{
#if DEBUG
  Serial.begin(115200);
#endif
  pinMode(TRIAC, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3), zero_crossing, RISING);
}

void loop()
{
  fonction123();
#if TEMP
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
#endif

#if DEBUG
  Serial.print(" Temperature= ");
  Serial.print(Tc);
  Serial.print(" ADC= ");
  Serial.println(analogRead(ThermistorPin));

#endif
}