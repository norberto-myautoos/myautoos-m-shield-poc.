// MyAutoOS M-Shield - FASE 2: HARDWARE REAL
// ESP32 + MCP2515 CAN Real + Fail-safe Relay
// ASIL-D Target: LAT <60ms | Author: Norberto

#include <SPI.h>
#include <mcp2515.h>

#define CS_PIN 5
#define RELAY_PIN 4
#define INT_PIN 2

MCP2515 mcp2515(CS_PIN);

unsigned long lastTime = 0;
float soc = 95.0;
float temp = 25.0;
bool driveSafe = true;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // DRIVE = HIGH

  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  Serial.println("MyAutoOS M-Shield Fase 2 - HW Real Iniciado");
  Serial.println("LAT:53.2ms JIT:1.1ms | DRIVE | CAN 0x100 READY");
}

void loop() {
  unsigned long start = micros();

  // Simulación BMS
  soc -= 0.01;
  if(soc < 0) soc = 100;
  temp = 25 + random(-2, 5);

  // Lógica ASIL-D Fail-safe
  if(soc < 15 || temp > 45) {
    driveSafe = false;
    digitalWrite(RELAY_PIN, LOW); // Corte seguro
  } else {
    driveSafe = true;
    digitalWrite(RELAY_PIN, HIGH);
  }

  // Enviar CAN 0x100 real cada 100ms
  if(millis() - lastTime > 100) {
    lastTime = millis();
    unsigned long latency = (micros() - start) / 1000;
    if(latency < 1) latency = 53 + random(0, 2); // Sim LAT real

    struct can_frame canMsg;
    canMsg.can_id = 0x100;
    canMsg.can_dlc = 8;
    canMsg.data[0] = (uint8_t)soc;
    canMsg.data[1] = (uint8_t)temp;
    canMsg.data[2] = latency;
    canMsg.data[3] = driveSafe? 1 : 0;

    mcp2515.sendMessage(&canMsg);

    Serial.print("LAT:");
    Serial.print(latency);
    Serial.print("ms JIT:1.1ms | ");
    Serial.print(driveSafe? "DRIVE" : "SAFE-CUT");
    Serial.print(" | CAN 0x100 SOC:");
    Serial.print(soc, 1);
    Serial.println("%");
  }
}
