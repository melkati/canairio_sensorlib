// --------------------------------------------------------------
// Example of low power mode for Sensirion SCD4x and SCD30 sensors
// --------------------------------------------------------------
//
// - HIGH_PERFORMANCE:
//
//    Sampling period 5 sec, power supply current @3.3V 15mA for SCD4x
//
// - BASIC_LOWPOWER (Supported by SCD40, SCD41 and SCD30):
//
//    Sampling period 30 sec, power supply current @3.3V 3.2mA for SCD4x
//
// - MEDIUM_LOWPOWER (Only supported by SCD41):
//
//    Idle single shot mode. Sampling period 60 sec, power supply current @3.3V 1.5mA for
//    SCD41. Sampling period 300 sec, power supply current @3.3V 450µA for SCD41
//
// - MAXIMUM_LOWPOWER (Only supported by SCD41):
//
//    Power-cycled single shot mode. Sampling period 600 sec (10 min), power supply current @3.3V
//    250µA. Sampling period 1200 sec (20 min), power supply current @3.3V 130µA. Sampling period
//    3600 sec (1 h), power supply current @3.3V 43µA´
//
// --------------------------------------------------------------

#include <Arduino.h>

#include "Sensors.hpp"

void printSensorsDetected() {
  uint16_t sensors_count = sensors.getSensorsRegisteredCount();
  uint16_t units_count = sensors.getUnitsRegisteredCount();
  Serial.println("-->[MAIN] Sensors detected count\t: " + String(sensors_count));
  Serial.println("-->[MAIN] Sensors units count  \t: " + String(units_count));
  Serial.print("-->[MAIN] Sensors devices names\t: ");
  int i = 0;
  while (sensors.getSensorsRegistered()[i++] != 0) {
    String deviceDetected = sensors.getSensorName((SENSORS)sensors.getSensorsRegistered()[i - 1]);
    Serial.print(deviceDetected);

    if (deviceDetected == "SCD4X") {
      Serial.println(" (" + sensors.scd4xModel + ")");
    }

    Serial.print(",,,,");
  }
  Serial.println();
}

void printSensorsValues() {
  Serial.println("-->[MAIN] Preview sensor values:");
  UNIT unit = sensors.getNextUnit();
  while (unit != UNIT::NUNIT) {
    String uName = sensors.getUnitName(unit);
    float uValue = sensors.getUnitValue(unit);
    String uSymb = sensors.getUnitSymbol(unit);
    Serial.println("-->[MAIN] " + uName + " \t: " + String(uValue) + " " + uSymb);
    unit = sensors.getNextUnit();
  }
}

void onSensorDataOk() {
  Serial.print("======= E X A M P L E   T E S T   L O W   P O W E R (");
  switch (sensors.getLowPowerMode()) {
    case HIGH_PERFORMANCE:
      Serial.print("HIGH_PERFORMANCE)");
      break;
    case BASIC_LOWPOWER:
      Serial.print("BASIC_LOWPOWER)");
      break;
    case MEDIUM_LOWPOWER:
      Serial.print("MEDIUM_LOWPOWER)");
      break;
    case MAXIMUM_LOWPOWER:
      Serial.print("MAXIMUM_LOWPOWER)");
      break;
    default:
      Serial.print("UNKNOWN_LOWPOWER_MODE)");
      break;
  }
  Serial.println(" =========");
  printSensorsDetected();
  printSensorsValues();
  Serial.println("=========================================");
}

void onSensorDataError(const char* msg) {}

/******************************************************************************
 *  M A I N
 ******************************************************************************/

void setup() {
  LowPowerModes powerMode = MAXIMUM_LOWPOWER;  // Choose HIGH_PERFORMANCE, BASIC_LOWPOWER,
                                               // MEDIUM_LOWPOWER, MAXIMUM_LOWPOWER
  Serial.begin(115200);
  delay(200);
  Serial.println("\n== Sensor test setup ==\n");

  Serial.println("-->[SETUP] Detecting sensors..");

  sensors.setOnDataCallBack(&onSensorDataOk);      // all data read callback
  sensors.setOnErrorCallBack(&onSensorDataError);  // [optional] error callback
  sensors.setDebugMode(true);                      // [optional] debug mode
  sensors.detectI2COnly(true);  // force to detect only i2c sensors (disable for UART sensors)
  sensors.setLowPowerMode(powerMode);

  switch (sensors.getLowPowerMode()) {
    case HIGH_PERFORMANCE:  // Use sensor's default ( 5 seconds for SCD4x and 2 sec for SCD30)
      break;
    case BASIC_LOWPOWER:          // Supported by SCD40, SCD41 and SCD30 (30 seconds)
      sensors.setSampleTime(30);  // measurement each 5 seconds
      break;
    case MEDIUM_LOWPOWER:         // Only supported by SCD41 (60 seconds)
      sensors.setSampleTime(60);  // measurement each 60 seconds. 1.5mA @ 3.3V
      break;
    case MAXIMUM_LOWPOWER:         // Only supported by SCD41 (600 seconds)
      sensors.setSampleTime(600);  // measurement each 600 seconds. 250µA @ 3.3V
      break;
  }

  // Init sensors. Choose only one of the following methods
  // sensors.init();  // Init all sensors in normal mode
  sensors.initCO2LowPowerMode(powerMode);  // Init CO2 sensor in low power mode
}

void loop() {
  sensors.loop();  // read sensor data and showed it
}
