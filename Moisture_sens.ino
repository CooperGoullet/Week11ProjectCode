#include <ArduinoBLE.h>
#include <DHT11.h>

const char* serviceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"; // Custom UUID for Moisture Service
const char* characteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";


int soilMoistureLevelS1;
int soilMoistureLevelS2;
DHT11 dht11(2);


BLEService moistureService(serviceUuid);
BLEIntCharacteristic moistureLevelChar(characteristicUuid, BLERead | BLENotify);

void setup() {
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  Serial.begin(9600);
  while (!Serial); 


  // Start BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set up BLE Service and Characteristic
  BLE.setLocalName("MoistureSensor");
  BLE.setAdvertisedService(moistureService);
  moistureService.addCharacteristic(moistureLevelChar);
  BLE.addService(moistureService);

  BLE.advertise();
  Serial.println("BLE device is now advertising");

}

void Teperature_and_Humidity() {
  int temperature = 0;
  int humidity = 0;
    
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.println(" %");

}

void Moisture_sensor1() {
  soilMoistureLevelS1 = analogRead(A1);
  Serial.print("Moisture S1: ");
  Serial.println(soilMoistureLevelS1);

  // Update BLE characteristic
  moistureLevelChar.writeValue(soilMoistureLevelS1);
}


void loop() {

  
  BLEDevice central = BLE.central();
  if(central)
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()){

      Teperature_and_Humidity();
      Moisture_sensor1();

      BLE.poll();
      delay(1000);

      if(moistureLevelChar.written()){

        if (moistureLevelChar.value()){
          BLE.poll();
          delay(1000);
        }
      }

    }

  }


  Serial.print("\n"); 

    // Moisture_sensor2();

}

 
//End of Code
