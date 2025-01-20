#include <BLEDevice.h>
#include <BLEServer.h>
#include "DFRobot_OxygenSensor.h"

// I2C Address and Constants
#define Oxygen_IICAddress ADDRESS_3  // Update based on your I2C address setting
#define COLLECT_NUMBER  10           // Number of samples to average
#define MOTOR_PIN 33                 // GPIO pin to control the motor

// BLE Configuration for Slave 2
#define SERVICE_UUID "6d25c5e1-2d8b-4f19-9ff8-8948e872b4d2"
#define CHARACTERISTIC_UUID_NOTIFY "12c72018-c1a3-469c-9839-e612aa6b400d" // Notify characteristic
#define CHARACTERISTIC_UUID_WRITE "6c7ed38e-d01c-43c8-b1e6-883fae54051b"  // Write characteristic

BLECharacteristic *pNotifyCharacteristic;
BLECharacteristic *pWriteCharacteristic;

DFRobot_OxygenSensor oxygen;

// Callback for Write Characteristic
class WriteCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        uint8_t *value = pCharacteristic->getData();
        int length = pCharacteristic->getLength();

        if (length > 0) {
            Serial.print("Received from Master: ");
            for (int i = 0; i < length; i++) {
                Serial.print(value[i]);
            }
            Serial.println();

            // Example: Check if command is "1"
            if (value[0] == '1') {
                Serial.println("Command received: Motor ON");
                digitalWrite(MOTOR_PIN, HIGH); // Turn motor ON
            }
        }
    }
};

void setup() {
    Serial.begin(115200);

    // Initialize oxygen sensor
    while (!oxygen.begin(Oxygen_IICAddress)) {
        Serial.println("I2C device number error!");
        delay(1000);
    }
    Serial.println("I2C connect success!");

    // Initialize motor pin as output
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW); // Ensure motor is off at startup

    // Initialize BLE
    BLEDevice::init("BLE-OxygenSensor");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Notify characteristic for sending oxygen data
    pNotifyCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_NOTIFY,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Write characteristic for receiving commands
    pWriteCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_WRITE,
        BLECharacteristic::PROPERTY_WRITE
    );

    pWriteCharacteristic->setCallbacks(new WriteCallback());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();

    Serial.println("BLE setup complete. Advertising oxygen data...");
}

void loop() {
    // Read oxygen concentration
    float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);

    if (isnan(oxygenData)) {
        Serial.println("Failed to read oxygen data!");
        return;
    }

    // Notify master with oxygen data
    pNotifyCharacteristic->setValue((uint8_t *)&oxygenData, sizeof(oxygenData));
    pNotifyCharacteristic->notify();
    delay(5000);

    // Control motor based on oxygen level
    if (oxygenData < 20.0) {
        digitalWrite(MOTOR_PIN, HIGH); // Turn motor ON
        Serial.print("Oxygen Level: ");
        Serial.println(oxygenData);
        Serial.println("Motor ON - Oxygen level is below 20%");
    } else {
        digitalWrite(MOTOR_PIN, LOW); // Turn motor OFF
        Serial.print("Oxygen Level: ");
        Serial.println(oxygenData);
        Serial.println("Motor OFF - Oxygen level is above 20%");
    }
}
