#include <BLEDevice.h>
#include <BLEServer.h>
#include <DHT.h>

// Pin definitions for LEDs
#define RED_LED_PIN 32   // GPIO pin for the red LED
#define BLUE_LED_PIN 33  // GPIO pin for the blue LED

// DHT sensor configuration
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// BLE Configuration for Slave 1
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_NOTIFY "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Notify
#define CHARACTERISTIC_UUID_WRITE "beb5483e-36e1-4688-b7f5-ea07361b26a9" // Write

BLECharacteristic *pNotifyCharacteristic;
BLECharacteristic *pWriteCharacteristic;

int LEDstatus = 1;

// Callback for write characteristic
class WriteCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        uint8_t *value = pCharacteristic->getData();
        if (value) {
            LEDstatus = *value; // Directly update LEDstatus
            Serial.printf("Received command: %d\n", LEDstatus);
        } else {
            Serial.println("Received invalid command!");
        }
    }
};



void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE with DHT22 sensor!");

    // Initialize DHT sensor
    dht.begin();

    // Initialize LEDs
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);  // Turn off LEDs initially
    digitalWrite(BLUE_LED_PIN, LOW);

    // Initialize BLE
    BLEDevice::init("BLE-DHT22");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Notify characteristic for sending sensor data
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

    Serial.println("BLE setup complete. Advertising DHT22 data...");
}

void loop() {
    float data[2];
    data[0] = dht.readTemperature();
    data[1] = dht.readHumidity();

    if (isnan(data[0]) || isnan(data[1])) {
        Serial.println("Failed to read from DHT22 sensor!");
        return;
    }

    // Control LEDs based on LEDstatus
    if (LEDstatus == 1) {
        digitalWrite(RED_LED_PIN, data[0] < 25.0 ? HIGH : LOW);
        digitalWrite(BLUE_LED_PIN, data[0] >= 25.0 ? HIGH : LOW);
        Serial.println(data[0] < 25.0 ? "Red LED ON" : "Blue LED ON");
    } else if (LEDstatus == 0) {
        digitalWrite(RED_LED_PIN, LOW);  // Turn off both LEDs
        digitalWrite(BLUE_LED_PIN, LOW);
        Serial.println("Both LEDs OFF");
    }

    // Notify the master with sensor data
    pNotifyCharacteristic->setValue((uint8_t *)data, sizeof(data));
    pNotifyCharacteristic->notify();
    Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", data[0], data[1]);

    delay(5000); // 5-second interval
}
