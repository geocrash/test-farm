/*
   Pin configuration
   Pump (PWM)                   D11
   Relay_Heater                 D9
   Pressure Sensor Inlet        A2 of ADC1115
   Pressure Sensor Outlet       A3 of ADC1115
   Temperature Sensor           D2
   ADCBoard 1115 SDA            A4
   ADCBoard 1115 SCL            A5
*/

#include <Adafruit_ADS1X15.h>                        // library for ads1115
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// Number of expected temperature sensors
#define EXPECTED_NUMBER_OF_DEVICES 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_ADS1115 ads;

char dataString[] = {0};                             // used to save data and print them as a string
float result [11];                                   // Used to save values of data logging
bool Logged = false;                                 // Makes sure that the logging happens only once during pulse
bool too_Hot = false;                                // Makes sure that water temperature remains under 45 degrees
bool p_cycled = true;                                // Counts how many pressure cycles
bool t_cycled = true;                                // Counts how many temperature cycles
bool t_counted = false;                              // Checks if temperature cycle already counted
bool cooling = false;
/*
bool day = true;
bool night = false;
bool first = true;
*/

// Pins
const int pwm = 11;                                   // Used for the pump
const int heater = 9;                                 // Used for the heater
// Temperature max and temperature min
const int T_Max = 37;                                 // Max Temperature of water bath
const int T_Min = 35;                                 // Min Temperature of water bath
// Pressures depending on the pwm signals
const int pwm1 = 120;                              // PWM which gives pressure min
const int pwm2 = 220;                              // PWM which gives pressure max
// Duration of each cycle
const int Log_Duration = 5000;                        // 5 second logging duration for low pulse period
unsigned long Pump_Start = 0;                         // Delay start of normal pump
// Number of cycles
long int p_cycle_count = 110700;
long int t_cycle_count = 14;
// Counting for heater shut off (MUST BE ADJUSTED, define in minutes)
/*
long int initial_time = 10 * 60;
*/

// Struct for logging after certain time
struct t {
    unsigned long tStart;
    unsigned long tTimeout;
};

t Log_Start = {0, Log_Duration};                      // Creates struct for logging once every 5 seconds during regular flow
t tsensorcheck = {0, 60000};
/*
t day_timer = {0, 28800000};
t night_timer = {0, 57600000};
t initial_timer = {0, 10000};
*/

int numberOfDevices; // Number of temperature devices found
bool sensorMissing = 0; // Errorflag to give a warning if not all expected sensors are connected.
int SerialNumbersOfDevices[EXPECTED_NUMBER_OF_DEVICES];

struct TempSensor {
  uint8_t deviceAddress[16];
  String serialNumber;
  String location;
  float tempValue;
  bool found;
};

struct TempSensor myTempSensors[EXPECTED_NUMBER_OF_DEVICES];
DeviceAddress tempDeviceAddress;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                                 // Starting Serial Communication
  pinMode(pwm, OUTPUT);                               // Pump pin behaving as output. It works through a PWM signal
  pinMode(heater, OUTPUT);                            // Heater pin behaving as output. It works as on/off through a relay
  ads.setGain(GAIN_TWOTHIRDS);                        // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
  ads.begin();
  sensors.begin();
  digitalWrite(heater, LOW);                         // Initially keeping heater off
  result[5] = 1;
  // Start of delay
  unsigned long Pump_Start = millis();                // Delay start of normal pump, delay of 590 seconds
  numberOfDevices = sensors.getDeviceCount();
  for (int i = 0; i < EXPECTED_NUMBER_OF_DEVICES; i++) {

    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {

      for (int g = 0; g < 8; g++) { // Das Array ist 8 chars lang
        myTempSensors[i].deviceAddress[g] = tempDeviceAddress[g];
      }

      myTempSensors[i].serialNumber = returnAddressAsString(myTempSensors[i].deviceAddress);

      /*
       * ENTER SERIAL CODES AND ACCORDING LOCATIONS (PSEUDO DATABASE)
       */
        if (myTempSensors[i].serialNumber == "28f4b476e0013c5f") {myTempSensors[i].location = "a1_pos_out_temp";}
        else if (myTempSensors[i].serialNumber == "28ea0876e0013c79") {myTempSensors[i].location = "a1_neg_out_temp";}
        else if (myTempSensors[i].serialNumber == "28a6d176e0013cdc") {myTempSensors[i].location = "a2_pos_out_temp";}
        else if (myTempSensors[i].serialNumber == "2891a276e0013c97") {myTempSensors[i].location = "a2_neg_out_temp";}

        else if (myTempSensors[i].serialNumber == "28c8a6280d0000f3") {myTempSensors[i].location = "a2_neg_out_temp";}
        else if (myTempSensors[i].serialNumber == "283820290d000059") {myTempSensors[i].location = "a2_neg_out_temp";}
        else if (myTempSensors[i].serialNumber == "28b3c0280d00001e") {myTempSensors[i].location = "a2_neg_out_temp";}
        else {
          myTempSensors[i].location = "unknwn";
        }
    }
    myTempSensors[i].found = sensors.getAddress(tempDeviceAddress, i); // is 1 if the device is found/connected. 0 if not.
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Reads pressure sensor
  int16_t adc2;                                     // Used for reading the analogue pressure sensor through ADS1115
  float volts2;                                   // Used for reading the analogue pressure sensor through ADS1115
  float Pressure2;                             // Used for finding pressure from voltage
  adc2 = ads.readADC_SingleEnded(2);                      // Finds analogue value of inlet pressure sensor
  volts2 = ads.computeVolts(adc2);                        // Finds digital value of pressure sensor
  Pressure2 = 0.3 * volts2;                               // Finds pressure (in bar) because each volt is 0,3bar

  // Reads temperature sensors
  sensors.requestTemperatures();
  float E_Temp = myTempSensors[0].tempValue = sensors.getTempC(myTempSensors[0].deviceAddress);                    // Reads the temperature sensor, for water
  sensors.requestTemperatures();
  float W_Temp = myTempSensors[1].tempValue = sensors.getTempC(myTempSensors[1].deviceAddress);                   // Reads the temperature sensor, for water

  // Left Hand Side For if condition simplification conditions of pump and heater
  unsigned long time_Pump = millis() - Pump_Start;        // Delay start of pulse, delay of 10 seconds

  // Controlling pump pressure!!!
  // Applies pressure min for 10 seconds
  if (time_Pump <= 10000) {
    analogWrite(pwm, pwm1);
    result[5] = pwm1;
    if (p_cycled == true){
      p_cycle_count = p_cycle_count + 1;
      p_cycled = false;
    }
  }
  // Applies pressure max for 10 seconds
  else if ((time_Pump > 10000) && (time_Pump <= 20000)) {
    analogWrite(pwm, pwm2);
    result[5] = pwm2;
  }
  // Resets time
  else if(time_Pump > 20000){
    p_cycled = true;
    Pump_Start += time_Pump;                              // resets time
  }

  // Controlling water bath temperature!!!
  /*
  if((initial_time < (17 * 60)) && (first == true)){
    day = true;
    initial_timer -> tTimeout = (17 * 60 * 60 * 1000) - (initial_time * 60 * 1000);
  }
  else{
    day = false;
    first = false;
  }

  if(tCheck(&initial_timer) && (first == true)){
    first = false;
    day = false;
  }

  if(day == true){
    if(tCheck(&day_timer)){
      tRun(&night_timer);
    }
  }
  */

  // If temperature sensors disconnects
  if((E_Temp == -127) || (W_Temp == -127)){
    W_Temp = 41.0;
    result[4] = -127;
  }

  // Safety, to make sure water temperature remains under 40 degrees
  if (W_Temp >= (T_Max + 3)){
    too_Hot = true;
    digitalWrite(heater, HIGH);        // Turns off heater
    result[4] = 0;
  }
  else if (W_Temp <= (T_Max + 1)){
    too_Hot = false;
  }

  // Maintains temperature of electrolyte between T_Max and T_Min
  if (E_Temp >= T_Max){
    digitalWrite(heater, HIGH);        // Turns off heater, when electrolyte temperature is more than 37
    result[4] = 0;
    t_counted = false;
    cooling = true;
  }
  else if ((E_Temp <= T_Max) && (E_Temp >= T_Min) && (cooling == false) && (too_Hot == false)){
    digitalWrite(heater, LOW);        // Turns on heater
    result[4] = 1;
  }
  if ((E_Temp < T_Min) && (too_Hot == false)){
    digitalWrite(heater, LOW);        // Turns on heater
    result[4] = 1;
    cooling = false;
    if (t_counted == false){
      t_cycle_count = t_cycle_count + 1;
      t_counted = true;
    }
  }

  // Data logging into Raspberry Pi in CSV
  if (tCheck(&Log_Start)) {
    // Data logging into Raspberry Pi in Excel
    result[0] = time_Pump / 1000;                           // Logs time position of pressure with regards to profile
    result[1] = Pressure2;                                  // Logs Pressure
    result[2] = W_Temp;                                     // Logs Water Temperature
    result[3] = E_Temp;                                     // Where to put second sensor?
    result[6] = p_cycle_count;                              // Temperature cycle count
    result[7] = p_cycle_count/250;
    result[8] = (p_cycle_count/250) * 3.75;
    result[9] = t_cycle_count;                             // Pressure cycle count
    result[10] = millis()/1000;
    Serial.print(result[0]); Serial.print(";"); Serial.print(result[1]); Serial.print(";"); Serial.print(result[2]); Serial.print(";");
    Serial.print(result[3]); Serial.print(";"); Serial.print(result[4]); Serial.print(";"); Serial.print(result[5]); Serial.print(";");
    Serial.print(result[6]); Serial.print(";"); Serial.print(result[7]); Serial.print(";"); Serial.print(result[8]); Serial.print(";");
    Serial.print(result[9]); Serial.print(";"); Serial.print(result[10]); Serial.println(";");
    tRun(&Log_Start);
  }

  // Reads temperature sensors
  sensors.requestTemperatures();
  E_Temp = myTempSensors[0].tempValue = sensors.getTempC(myTempSensors[0].deviceAddress);                    // Reads the temperature sensor, for water
  sensors.requestTemperatures();
  W_Temp = myTempSensors[1].tempValue = sensors.getTempC(myTempSensors[1].deviceAddress);

  adc2 = ads.readADC_SingleEnded(2);                      // Finds analogue value of inlet pressure sensor
  volts2 = ads.computeVolts(adc2);                        // Finds digital value of pressure sensor
  Pressure2 = 0.3 * volts2;                                // Finds pressure (in bar) because each volt is 0,4bar

  // safety in case something went wrong, 3 minute buffer
  if (((millis() > 300000) && (Pressure2 < 0.1)) || ((millis() > 300000) && E_Temp > 42) || ((millis() > 300000) && W_Temp > 45)){
    delay(1000);
    result[0] = time_Pump / 1000;                           // Logs time position of pressure with regards to profile
    result[1] = Pressure2;                                  // Logs Pressure
    result[2] = W_Temp;                                     // Logs Water Temperature
    result[3] = E_Temp;                                     // Where to put second sensor?
    result[6] = p_cycle_count;                              // Temperature cycle count
    result[7] = p_cycle_count/250;
    result[8] = (p_cycle_count/250) * 3.75;
    result[9] = t_cycle_count;                             // Pressure cycle count
    result[10] = millis()/1000;
    Serial.print(result[0]); Serial.print(";"); Serial.print(result[1]); Serial.print(";"); Serial.print(result[2]); Serial.print(";");
    Serial.print(result[3]); Serial.print(";"); Serial.print(result[4]); Serial.print(";"); Serial.print(result[5]); Serial.print(";");
    Serial.print(result[6]); Serial.print(";"); Serial.print(result[7]); Serial.print(";"); Serial.print(result[8]); Serial.print(";");
    Serial.print(result[9]); Serial.print(";"); Serial.print(result[10]); Serial.println(";");

    if(Pressure2 < 0.1){
      delay(1000);
      Serial.println("Pressure too low!! Shutting down TestFarm1!!");
      analogWrite(pwm, 0);
      digitalWrite(heater, HIGH);
    }

    if(E_Temp > 42){
      delay(1000);
      Serial.println("Electrolyte Temperature too high!! Shutting down TestFarm1!!");
      analogWrite(pwm, 0);
      digitalWrite(heater, HIGH);
    }

    if(W_Temp > 45){
      delay(1000);
      Serial.println("Water Temperature too high!! Shutting down TestFarm1!!");
      analogWrite(pwm, 0);
      digitalWrite(heater, HIGH);
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

String returnAddressAsString(DeviceAddress deviceAddress) {
  String completeDeviceAddress;
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) {
      completeDeviceAddress += String(0, HEX);
    }
    completeDeviceAddress += String(deviceAddress[i], HEX);
  }
  return completeDeviceAddress;
}
