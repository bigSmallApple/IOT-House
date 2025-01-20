#define CONFIG_BT_NIMBLE_TASK_STACK_SIZE 8192 #include <NimBLEDevice.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>
//---------------------------------------- // Definitions and libraries for xbee
// library & pins for uart communication #include <HardwareSerial.h>
#define RX_pin 16 //rx pin #define TX_pin 17 //tx pin HardwareSerial xbeeSerial(2);
// pins for output
#define BedRoom_Xbee2_fan_act 12 //xbee with the tmp36 sensor and fan actuator #define BedRoom_Xbee3_led_act 14 // xbee with the occupacy sensor and led
#define Storage_Xbee4_fan_act 15 //xbee that is in the storage room with the ventalation
// pins for input
#define Bedroom_Xbee3_occ_sen 13 //xbee with the occupancy sensor and led #define Door_Xbee5_occ_sen 18 //xbee that is at the door monitoring it //-------------------------------------------
// WiFi Credentials
const char *ssid = "Sneaky";
const char *password = "sneaky123";
// Firebase Project Credentials
#define FIREBASE_HOST "https://smarthome-b2cca-default-rtdb.firebaseio.com" #define FIREBASE_AUTH "ubwfg5r84yErYdGXiKGtnBMR6YJ3wdXKdzT4ZIIr"
float hometemp = 0;
float homehumidity = 0; bool hasHomeData = false;
float kitchenOxygen = 0; float oldOxy = 0;
bool hasKitchenData = false;
String btnHVAC = "ON"; Servo myServo;
int servoPin = 27; #define PIR_PIN 32 #define LED_PIN 25
Wed, Dec 11 at 2:06 PM
  
// Variables for motion sensor and motor bool motionDetected = false;
unsigned long motorStartTime = 0;
bool motorActive = false;
String garageDoorStatus = "CLOSED"; // Default state
// Firebase configuration and objects FirebaseConfig config;
FirebaseAuth auth; FirebaseData firebaseData;
// BLE UUIDs for Slave 1 (Temperature and Humidity)
static NimBLEUUID serviceUUID1("4fafc201-1fb5-459e-8fcc-c5c9c331914b"); static NimBLEUUID charUUID1("beb5483e-36e1-4688-b7f5-ea07361b26a8"); static NimBLEUUID charWriteUUID1("beb5483e-36e1-4688-b7f5-ea07361b26a9");
// BLE UUIDs for Slave 2 (Oxygen Sensor)
static NimBLEUUID serviceUUID2("6d25c5e1-2d8b-4f19-9ff8-8948e872b4d2"); static NimBLEUUID charUUID2("12c72018-c1a3-469c-9839-e612aa6b400d");
// Variables for Slave Devices
NimBLERemoteCharacteristic *pRemoteCharacteristic1 = nullptr; NimBLERemoteCharacteristic *pWriteCharacteristic1 = nullptr; NimBLEAdvertisedDevice *slave1Device = nullptr;
bool connectToSlave1 = false;
NimBLERemoteCharacteristic *pRemoteCharacteristic2 = nullptr; NimBLEAdvertisedDevice *slave2Device = nullptr;
bool connectToSlave2 = false;
void initializeServo() {
myServo.attach(servoPin); // Attach the servo to its pin Serial.println("Servo initialized.");
}
//to initialize xbee comunication void setUpXbee(){
xbeeSerial.begin(9600, SERIAL_8N1, RX_pin, TX_pin);
Serial.println("Xbee communication secured"); }
float retrieveXbeeData(){
if (xbeeSerial.available()){
String data = xbeeSerial.readStringUntil('\n');
return data.toFloat(); }
return 0.0; }
// Firebase Initialization void initializeFirebase() {
config.host = FIREBASE_HOST; config.signer.tokens.legacy_token = FIREBASE_AUTH; Firebase.begin(&config, &auth); Firebase.reconnectWiFi(true);
Serial.println("Firebase initialized.");
}

// Notification callback for Slave 1 (DHT22)
void notifyCallback1(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
if (length != sizeof(float) * 2) { Serial.println("Invalid data length from Slave 1!"); return;
}
memcpy(&hometemp, &pData[0], sizeof(float)); memcpy(&homehumidity, &pData[sizeof(float)], sizeof(float));
Serial.printf("Slave 1 - Temperature: %.2f, Humidity: %.2f\n", hometemp, homehumidity);
hasHomeData = true; }
// Notification callback for Slave 2 (Oxygen Sensor)
void notifyCallback2(NimBLERemoteCharacteristic *pRemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
if (length != sizeof(float)) {
Serial.println("Invalid data length from Slave 2!"); return;
}
memcpy(&kitchenOxygen, &pData[0], sizeof(float)); Serial.printf("Slave 2 - Oxygen Level: %.2f\n", kitchenOxygen);
hasKitchenData = true; }
// Connect to a BLE slave
bool connectToSlave(NimBLEAdvertisedDevice *device, NimBLERemoteCharacteristic **pNotifyCharacteristic, NimBLEUUID serviceUUID, NimBLEUUID charUUID, void (*notifyCallback)(NimBLERemoteCharacteristic *, uint8_t *, size_t, bool)) {
NimBLEClient *pClient = NimBLEDevice::createClient(); if (!pClient->connect(device)) {
Serial.println("Failed to connect to Slave!");
return false; }
Serial.println("Connected to Slave!");
NimBLERemoteService *pRemoteService = pClient->getService(serviceUUID); if (!pRemoteService) {
Serial.println("Failed to find remote service!"); pClient->disconnect();
return false;
}
Serial.println("Remote service found!");
*pNotifyCharacteristic = pRemoteService->getCharacteristic(charUUID); if (!*pNotifyCharacteristic) {
Serial.println("Failed to find notify characteristic!"); pClient->disconnect();
return false;
}
if ((*pNotifyCharacteristic)->canNotify()) { (*pNotifyCharacteristic)->subscribe(true, notifyCallback);

Serial.println("Subscribed to notifications!"); } else {
Serial.println("Notify characteristic cannot notify!"); }
// Assign the write characteristic (specific to Slave 1) if (device == slave1Device) {
pWriteCharacteristic1 = pRemoteService->getCharacteristic(charWriteUUID1); if (pWriteCharacteristic1) {
Serial.println("Write characteristic for Slave 1 found!"); } else {
Serial.println("Failed to find write characteristic for Slave 1!"); }
}
return true; }
// Advertised device callback
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
void onResult(NimBLEAdvertisedDevice *advertisedDevice) { if (advertisedDevice->isAdvertisingService(serviceUUID1)) {
if (slave1Device == nullptr) { slave1Device = advertisedDevice; connectToSlave1 = true; Serial.println("Slave 1 identified!");
}
} else if (advertisedDevice->isAdvertisingService(serviceUUID2)) {
if (slave2Device == nullptr) { slave2Device = advertisedDevice; connectToSlave2 = true; Serial.println("Slave 2 identified!");
} }
} };
// Firebase Upload Function void uploadToFirebase() {
if (hasHomeData) {
FirebaseJson homeData;
homeData.add("temp", hometemp);
homeData.add("humidity", homehumidity); homeData.add("ac_status", hometemp < 25 ? "OFF" : "ON"); homeData.add("heater_status", hometemp >= 25 ? "OFF" : "ON");
Firebase.pushJSON(firebaseData, "/home", homeData); hasHomeData = false;
Serial.println("Home data uploaded to Firebase.");
}
if (hasKitchenData) {
FirebaseJson kitchenData;
if(oldOxy != kitchenOxygen){
kitchenData.add("oxygenlevel", kitchenOxygen); kitchenData.add("chimneystatus", kitchenOxygen < 20 ? "ON" : "OFF");

Firebase.pushJSON(firebaseData, "/kitchen", kitchenData); hasKitchenData = false;
Serial.println("Kitchen data uploaded to Firebase."); oldOxy = kitchenOxygen;
}
} }
// Send command to Slave 1
void sendCommandToSlave1(uint8_t command) {
if (pWriteCharacteristic1) {
Serial.println("Write characteristic found for Slave 1."); if (pWriteCharacteristic1->canWrite()) {
Serial.println("Write characteristic is writable. Sending command..."); pWriteCharacteristic1->writeValue(&command, sizeof(command)); Serial.printf("Sent command to Slave 1: %d\n", command);
} else {
Serial.println("Write characteristic is not writable!");
}
} else {
Serial.println("Write characteristic for Slave 1 not found!"); }
}
void initializeHardware() {
// Initialize motor, motion sensor, and LED pinMode(PIR_PIN, INPUT); pinMode(LED_PIN, OUTPUT);
// Ensure motor is off initially
digitalWrite(LED_PIN, LOW); // Ensure LED is off initially // Initialize XBee input and output pins
// Input pins
pinMode(Bedroom_Xbee3_occ_sen, INPUT); pinMode(Door_Xbee5_occ_sen, INPUT);
// Output pins
pinMode(BedRoom_Xbee2_fan_act, OUTPUT); pinMode(BedRoom_Xbee3_led_act, OUTPUT); pinMode(Storage_Xbee4_fan_act, OUTPUT);
}
void handleMotionSensor() { motionDetected = digitalRead(PIR_PIN);
if (motionDetected) {
digitalWrite(LED_PIN, HIGH); // Turn on LED Firebase.setString(firebaseData, "mainOp", "ON"); Serial.println("Motion detected: ON");
} else {
digitalWrite(LED_PIN, LOW); // Turn off LED Firebase.setString(firebaseData, "mainOP", "OFF"); Serial.println("No motion: OFF");
} }

void handleMotorControl() {
if (Firebase.getString(firebaseData, "btnGrage")) {
String buttonState = firebaseData.stringData(); Serial.print("Garage Button State: "); Serial.println(buttonState);
if (buttonState == "OPEN") {
// Turn motor/servo to simulate anticlockwise (OPEN) Serial.println("Sweeping servo ANTICLOCKWISE (OPEN)..."); for (int angle = 180; angle >= 0; angle -= 20) {
myServo.write(angle); // Move servo to the current angle
delay(500); // Wait for the servo to reach the position }
// Update Firebase to prevent repeat action
if (Firebase.setString(firebaseData, "btnGrage", "NOT OPEN")) {
Serial.println("Garage button state set to NOT OPEN."); } else {
Serial.print("Failed to update garage button state: ");
Serial.println(firebaseData.errorReason()); }
garageDoorStatus = "OPEN";
} else if (buttonState == "CLOSE") {
// Turn motor/servo to simulate clockwise (CLOSE) Serial.println("Sweeping servo CLOCKWISE (CLOSE)..."); for (int angle = 0; angle <= 180; angle += 20) {
myServo.write(angle); // Move servo to the current angle
delay(500); // Wait for the servo to reach the position }
// Update Firebase to prevent repeat action
if (Firebase.setString(firebaseData, "btnGrage", "NOT CLOSE")) {
Serial.println("Garage button state set to NOT CLOSE."); } else {
Serial.print("Failed to update garage button state: ");
Serial.println(firebaseData.errorReason()); }
garageDoorStatus = "CLOSED"; }
} else {
Serial.print("Failed to get garage button state: "); Serial.println(firebaseData.errorReason());
} }
void setup() { Serial.begin(115200);
// Connect to Xbee uart setUpXbee();

// Connect to WiFi
WiFi.begin(ssid, password); Serial.print("Connecting to WiFi");
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print("."); }
Serial.println("\nWiFi connected!");
// Initialize Firebase initializeFirebase();
// Initialize BLE
NimBLEDevice::init("BLE-Master");
NimBLEScan *pBLEScan = NimBLEDevice::getScan(); pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); pBLEScan->setActiveScan(true);
pBLEScan->start(15, false);
Serial.println("BLE Scanning...");
initializeHardware(); initializeServo();
}
void loop() {
handleMotionSensor(); handleMotorControl();
float xbeeTemperature = hometemp; xbeeTemperature -= 0.5;
if (xbeeTemperature > 25) { digitalWrite(BedRoom_Xbee2_fan_act, HIGH);
} else {
digitalWrite(BedRoom_Xbee2_fan_act, LOW);
}
Firebase.setString(firebaseData, "bedTmp", String(xbeeTemperature));
if (digitalRead(Bedroom_Xbee3_occ_sen) == HIGH) { digitalWrite(BedRoom_Xbee3_led_act, HIGH); Firebase.setString(firebaseData, "bedOp", "ON");
} else {
digitalWrite(BedRoom_Xbee3_led_act, LOW); Firebase.setString(firebaseData, "bedOp", "OFF");
}
if (kitchenOxygen < 20) {
digitalWrite(Storage_Xbee4_fan_act, HIGH); // Turn on the storage fan
Firebase.setString(firebaseData, "stoFan", "OFF"); } else {
digitalWrite(Storage_Xbee4_fan_act, LOW); // Turn off the storage fan
Firebase.setString(firebaseData, "stoFan", "ON"); }

if (connectToSlave1) {
Serial.println("Attempting to reconnect to Slave 1...");
if (!connectToSlave(slave1Device, &pRemoteCharacteristic1, serviceUUID1, charUUID1, notifyCallback1)) {
delay(2000); // Delay before retrying } else {
connectToSlave1 = false; // Connection successful
Serial.println("Reconnected to Slave 1 successfully!"); }
}
if (!connectToSlave1 && pWriteCharacteristic1) { uint8_t command = (btnHVAC == "ON") ? 1 : 0; sendCommandToSlave1(command);
} else if (!connectToSlave1) {
Serial.println("Reconnecting to Slave 1 to ensure write characteristic availability..."); connectToSlave(slave1Device, &pRemoteCharacteristic1, serviceUUID1, charUUID1, notifyCallback1);
}
if (Firebase.getString(firebaseData, "btnHVAC")) { btnHVAC = firebaseData.stringData(); Serial.print("Button Command for AC: "); Serial.println(btnHVAC);
} else {
Serial.print("Failed to get value for btnac: "); Serial.println(firebaseData.errorReason());
}
if (connectToSlave2) {
Serial.println("Attempting to reconnect to Slave 2...");
if (!connectToSlave(slave2Device, &pRemoteCharacteristic2, serviceUUID2, charUUID2, notifyCallback2)) {
delay(2000); // Delay before retrying } else {
connectToSlave2 = false; // Connection successful
Serial.println("Reconnected to Slave 2 successfully!"); }
}
// Upload data to Firebase uploadToFirebase();
//
// Handle motor control
delay(2000); }