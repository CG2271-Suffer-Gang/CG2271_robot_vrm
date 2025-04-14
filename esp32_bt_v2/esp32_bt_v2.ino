#include "BluetoothSerial.h"

#define RXD2 16
#define TXD2 17


BluetoothSerial SerialBT;  // Create Bluetooth Serial object

enum BTCommand {
  STOP = 0,
  FRONT = 1,
  BACK = 2,
  LEFT = 3,
  RIGHT = 4,
  FRONTLEFT = 5,
  FRONTRIGHT = 6,
  BACKLEFT = 7,
  BACKRIGHT = 8,
  SLOW = 9,
  FAST = 10,
  VICTORY = 11
};

bool isConnected = false;  // Track connection status

// Callback function for connection events
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    if (event == ESP_SPP_SRV_OPEN_EVT) {  // Device connected
        Serial.println("Bluetooth Device Connected!");
        isConnected = true;
        digitalWrite(2,HIGH);
    } 
    else if (event == ESP_SPP_CLOSE_EVT) {  // Device disconnected
        Serial2.write(0x35); //immediately stop car
        Serial.println("Bluetooth Device Disconnected!");
        isConnected = false;
        digitalWrite(2,LOW);

        // Restart Bluetooth so phone can find it again
        // Serial.println("Restarting Bluetooth...");
        // SerialBT.end();            // Stop Bluetooth
        // delay(500);                // Short delay to avoid issues
        //SerialBT.begin("ESP32_BT"); // Restart Bluetooth with the same name
    }
}

void setup() {
    Serial.begin(115200);   // Start serial monitor
    SerialBT.register_callback(btCallback);
    SerialBT.begin("ESP32_CG2271_SufferGang"); // Set Bluetooth device name
    Serial.println("Bluetooth Started! Ready to pair.");
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

    Serial2.write(0x35);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
}

void loop() {
    if (SerialBT.available()) {
        int8_t command = (int8_t) SerialBT.read();
        Serial.print("Received: ");
        Serial.println((BTCommand)command);
        
        // Process command
        switch ((BTCommand)command){
          case FRONT:
            Serial2.write(0x33);
            Serial.println("FRONT");
            break;
          case BACK:
            Serial2.write(0x32);
            Serial.println("BACK");
            break;
          case LEFT:
            Serial2.write(0x31);
            Serial.println("LEFT");
            break;
          case RIGHT:
            Serial2.write(0x30);
            Serial.println("RIGHT");
            break;
          case FRONTLEFT:
            Serial2.write(0x34);
            break;
          case FRONTRIGHT:
            Serial2.write(0x36);
            break;
          case BACKLEFT:
            Serial2.write(0x37);
            break;
          case BACKRIGHT:
            Serial2.write(0x38);
            break;
          case STOP:
            Serial2.write(0x35);
            break;
          case SLOW:
            Serial2.write(0x39);
            break;
          case FAST:
            Serial2.write(0x40);
            break;
          case VICTORY:
            Serial2.write(0x41);
            break;
          default:
            ;
            break;
        }
    }
}

