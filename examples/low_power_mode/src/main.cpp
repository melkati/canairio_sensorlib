#include <Arduino.h>

#include <Sensors.hpp>

// Return sensor mode: SCD40 or SCD41 (also SCD42 obsolete and unsupported)
String getSCD4xType(uint16_t featureSet) {
  uint8_t typeOfSensor = ((featureSet & 0x1000) >> 12);
  return "SCD4" + String(typeOfSensor);
}

// To move into the sensorlib
// 13th bit of the featureSet is the type of sensor
uint16_t getSCD4xFeatureSet() {
  uint16_t featureSet = 0;
  uint16_t error = 0;
  error = sensors.scd4x.getFeatures(featureSet);
  if (error != 0) {
    Serial.println("-->[MAIN] SCD4x getFeatures error: " + String(error));
  }
  return featureSet;
}

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
      uint16_t featureSet = getSCD4xFeatureSet();
      uint8_t typeOfSensor = ((featureSet & 0x1000) >> 12);
      Serial.println(" (SCD4" + getSCD4xType(featureSet) + ")");
    }

    Serial.print(",");
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
  Serial.println("======= E X A M P L E   T E S T =========");
  printSensorsDetected();
  printSensorsValues();
  Serial.println("=========================================");
}

void onSensorDataError(const char* msg) {}

/******************************************************************************
 *  M A I N
 ******************************************************************************/

LowPowerMode lowPowerMode = NO_LOWPOWER;  // Choose one of the low power modes: NO_LOWPOWER,
                                          // BASIC_LOWPOWER, MEDIUM_LOWPOWER, MAXIMUM_LOWPOWER

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n== Sensor test setup ==\n");

  Serial.println("-->[SETUP] Detecting sensors..");

  sensors.setOnDataCallBack(&onSensorDataOk);      // all data read callback
  sensors.setOnErrorCallBack(&onSensorDataError);  // [optional] error callback
  sensors.setDebugMode(true);                      // [optional] debug mode
  sensors.detectI2COnly(true);  // force to detect only i2c sensors (disable for UART sensors)
  sensors.setLowPowerMode(NO_LOWPOWER);  // set the chosen low power mode

  switch (sensors.getLowPowerMode()) {
    // High performance mode. Sampling period 5 sec, power supply current @3.3V 15mA for SCD4x
    case NO_LOWPOWER:
      sensors.setSampleTime(2);  // config sensors sample time interval. Only works for SCD30
      sensors.init();            // Auto detection to UART and i2c sensors
      break;

      // Low power mode. Sampling period 30 sec, power supply current @3.3V 3.2mA for SCD4x
    case BASIC_LOWPOWER:
      sensors.setSampleTime(30);
      sensors.init();
      break;

      // Only supported by SCD41
      // Idle single shot mode. Sampling period 60 sec, power supply current @3.3V 1.5mA for SCD41.
      // Sampling period 300 sec, power supply current @3.3V 450µA for SCD41
    case MEDIUM_LOWPOWER:
      break;

      // Only supported by SCD41
      // Power-cycled single shot mode. Sampling period 600 sec (10 min), power supply current @3.3V
      // 250µA. Sampling period 1200 sec (20 min), power supply current @3.3V 130µA. Sampling period
      // 3600 sec (1 h), power supply current @3.3V 43µA
    case MAXIMUM_LOWPOWER:
      break;
    default:
      break;
  }
}

void loop() {
  sensors.loop();  // read sensor data and showed it
}
