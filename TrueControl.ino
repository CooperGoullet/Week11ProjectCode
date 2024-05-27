#include <ArduinoBLE.h>

const int relayPin = 2; // Define the pin used to control the relay module
const char* peripheralName = "MoistureSensor"; // Peripheral's local name
const char* serviceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"; // Service UUID
const char* characteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"; // Characteristic UUID

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  pinMode(relayPin, OUTPUT); // Set the relay pin as an OUTPUT pin

  BLE.begin();

  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");

}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (strcmp(peripheral.localName().c_str(), peripheralName) != 0) {
      Serial.println("Peripheral name doesn't match, continuing scan...");
      return;
    }

    BLE.stopScan();

    controlRelay(peripheral);

    BLE.scanForUuid(serviceUuid);
  } else {
    Serial.println("No peripherals found, continuing scan...");
  }

  delay(1000); // Add a delay to avoid flooding the serial output
}

void controlRelay(BLEDevice peripheral) {
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic moistureLevelChar = peripheral.characteristic(characteristicUuid);

  if (!moistureLevelChar) {
    Serial.println("Peripheral does not have the required characteristic!");
    peripheral.disconnect();
    return;
  } else if (!moistureLevelChar.canRead()) {
    Serial.println("Peripheral does not have a readable characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    uint32_t moistureLevel;
    if (moistureLevelChar.readValue(moistureLevel)) {
      Serial.print("Moisture Level: ");
      Serial.println(moistureLevel);

      if (moistureLevel < 300) {
        digitalWrite(relayPin, HIGH);
        Serial.println("Relay ON");
      } else {
        digitalWrite(relayPin, LOW);
        Serial.println("Relay OFF");
      }
    } else {
      Serial.println("Failed to read characteristic value");
    }

    delay(1000);
  }

  Serial.println("Peripheral disconnected");
  peripheral.disconnect();
}




