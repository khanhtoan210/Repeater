#include <Arduino.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
// #include <Arduino_FreeRTOS.h>
// #include <task.h>
RF24 radio(9, 10);

const uint64_t pipe1 = 0xF0F0F0F0A1;//tx
const uint64_t pipe2 = 0xF0F0F0F0A3;//rx

uint16_t lastCounter = 0;
uint8_t lastParity = 0;

struct Payload {
  uint8_t parity;
  uint16_t counter;
  byte data;
};

Payload payload;
bool isValidPacket(Payload& payload);
void updateLastPacket(Payload& payload);
void receiverTask(void* parameter);
void transmitterTask(void* parameter);

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }
  radio.openReadingPipe(1, pipe1);//1034834473121ULL
  radio.openWritingPipe(pipe2);//1034834473122ULL
  radio.setChannel(80);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  // xTaskCreate(receiverTask, "Receiver Task", 10000, NULL, 1, NULL);
  // xTaskCreate(transmitterTask, "Transmitter Task", 10000, NULL, 1, NULL);

  // vTaskStartScheduler();
}
void loop() {
  radio.startListening();
  radio.read(&payload, sizeof(payload));
  Serial.print("Counter: "); Serial.print(payload.counter); Serial.print(' ');
  Serial.print("Parity: "); Serial.print(payload.parity); Serial.print(' ');
  Serial.print("Data: "); Serial.print(payload.data);
  delay(50);
  if (isValidPacket(payload)) {
    updateLastPacket(payload);
    radio.stopListening();
    bool ok = radio.write(&payload, sizeof(payload));
    Serial.print(" - ACK: ");
    Serial.println(ok);
  } else {
    Serial.println("Duplicate packet ignored");
  }  
  delay(50);

}
bool isValidPacket(Payload& payload) {
  return ((payload.counter > lastCounter && payload.parity == lastParity) ||
          (payload.counter < lastCounter && payload.parity != lastParity));
}
void updateLastPacket(Payload& payload) {
  lastCounter = payload.counter;
  lastParity = payload.parity;
}
// void receiverTask(void *pvParameters) {

//   while (radio.available()) {
//     radio.startListening();
//     radio.read(&payload, sizeof(payload));
//     Serial.print("Counter: "); Serial.print(payload.counter); Serial.print(' ');
//     Serial.print("Parity: "); Serial.print(payload.parity); Serial.print(' ');
//     Serial.print("Data: "); Serial.println(payload.data);
//     vTaskDelay(50 / portTICK_PERIOD_MS);
//   }
// }
// void transmitterTask(void *pvParameters) {
//   while(1){
//     if (isValidPacket(payload)) {
//       updateLastPacket(payload);
//       radio.stopListening();
//       bool ok = radio.write(&payload, sizeof(payload));
//       Serial.print(" - ACK: ");
//       Serial.println(ok);
//     } else {
//       Serial.println("Duplicate packet ignored");
//     }  
//     vTaskDelay(50 / portTICK_PERIOD_MS);
//   }
  
// }
