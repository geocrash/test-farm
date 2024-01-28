/*
   Pin configuration
   Pump (On/Off)                D11
   Pressure Sensor Inlet        A2 of ADC1115
   ADCBoard 1115 SDA            A4
   ADCBoard 1115 SCL            A5
*/

#include <Adafruit_ADS1X15.h>                        // library for ads1115
Adafruit_ADS1115 ads;

float result [4];                                   // Used to save values of data logging
bool Logged = false;                                 // Makes sure that the logging happens only once during pulse

// Pins
const int pump = 11;                                   // Used for the pump
// Duration of each cycle
const int Log_Duration = 30000;                        // 5 second logging duration for low pulse period

// Struct for logging after certain time
struct t {
    unsigned long tStart;
    unsigned long tTimeout;
};

t Log_Start = {0, Log_Duration};                      // Creates struct for logging once every 5 seconds during regular flow

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                                 // Starting Serial Communication
  pinMode(pwm, OUTPUT);                               // Pump pin behaving as output. It works through a PWM signal
  ads.setGain(GAIN_TWOTHIRDS);                        // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
  ads.begin();
  digitalWrite(pump, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Reads pressure sensor
  int16_t adc2;                                     // Used for reading the analogue pressure sensor through ADS1115
  float volts2;                                   // Used for reading the analogue pressure sensor through ADS1115
  float Pressure2;                             // Used for finding pressure from voltage
  adc2 = ads.readADC_SingleEnded(2);                      // Finds analogue value of inlet pressure sensor
  volts2 = ads.computeVolts(adc2);                        // Finds digital value of pressure sensor
  Pressure2 = 0.4 * volts2;                               // Finds pressure (in bar) because each volt is 0,3bar

  // Data logging into Raspberry Pi in CSV
  if (tCheck(&Log_Start)) {
    // Data logging into Raspberry Pi in Excel
    result[0] = millis() / 1000;                            // Logs real time
    result[1] = Pressure2;                                  // Logs Pressure
    result[2] = (millis() * 3) / 1000;
    result[3] = (result[2] / 1368750) * 100;
    Serial.print(result[0]); Serial.print(";"); Serial.print(result[1]); Serial.print(";"); Serial.print(result[2]); Serial.print(";");
    Serial.print(result[3]); Serial.println(";");
    tRun(&Log_Start);
  }

  adc2 = ads.readADC_SingleEnded(2);                      // Finds analogue value of inlet pressure sensor
  volts2 = ads.computeVolts(adc2);                        // Finds digital value of pressure sensor
  Pressure2 = 0.4 * volts2;                                // Finds pressure (in bar) because each volt is 0,4bar

  // safety in case something went wrong, 3 minute buffer
  if ((millis() > 300000) && (Pressure2 < 0.1)){
    delay(1000);
    result[0] = millis() / (1000 * 60);                            // Logs real time
    result[1] = Pressure2;                                  // Logs Pressure
    result[2] = (millis() * 3) / (1000 * 60);
    result[3] = (result[2] / 1368750) * 100;
    Serial.print(result[0]); Serial.print(";"); Serial.print(result[1]); Serial.print(";"); Serial.print(result[2]); Serial.print(";");
    Serial.print(result[3]); Serial.println(";");
    tRun(&Log_Start);

    if(Pressure2 < 0.1){
      delay(1000);
      Serial.println("Pressure too low!! Shutting down TestFarm1!!");
      digitalWrite(pump, HIGH);
    }
    delay(1000);
    while(1) {};
  }
}

// Functions for calling recording every certain time interval
bool tCheck (struct t *t ) {
    if (millis() > t->tStart + t->tTimeout) return true;
    else return false;
}

void tRun (struct t *t) {
    t->tStart = millis();
}
