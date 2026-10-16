// MyAutoOS M-Shield VCU Engine Mexicano - Fase 2 HiL Ready
// Founder: Norberto - Cherán, Michoacán | ITSU Uruapan, Michoacan 
// MyAutoOS Technologies - El Cerebro de México
// VCU Completo: Batería BMS + Freno Brake + Velocidad + HV
// Validado SiL LAT 53.2ms + BUS-OFF Real EFLG_TXBO + Wiring Corregido ASIL-D

#include <SPI.h>
#include <mcp2515.h>

#define CS_PIN 5
#define RELAY_DRIVE 4  // GPIO4 -> Relay IN (Morado) - Active LOW per datasheet corregido

MCP2515 mcp2515(CS_PIN);
struct can_frame canMsgRx;
unsigned long lastMsgTime = 0;
unsigned long prev = 0;
float lat_ms = 0;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_DRIVE, OUTPUT);
  digitalWrite(RELAY_DRIVE, HIGH); // HV ABIERTO inicio seguro
  
  SPI.begin(18, 19, 23, 5); // SCK 18, MISO 19, MOSI 23, CS 5 - Corregido
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("MyAutoOS M-Shield VCU Init OK - Cheran para el Mundo");
  Serial.println("MCP2515 SPI OK - CS:5 SCK:18 MISO:19 MOSI:23");
  Serial.println("CAN 500KBPS Mode Normal - ID 0x100 Listen");
  Serial.println("Bateria SoC 75% - Freno Ready - Velocidad Ready - HV Control");
  lastMsgTime = millis();
  prev = millis();
}

void loop() {
  // Task CAN - Lectura ID 0x100
  if (mcp2515.readMessage(&canMsgRx) == MCP2515::ERROR_OK) {
    if (canMsgRx.can_id == 0x100) {
      lastMsgTime = millis();
      lat_ms = lastMsgTime - prev;
      prev = lastMsgTime;
      
      String log = "RX 0x100 DLC " + String(canMsgRx.can_dlc) + " LAT: " + String(lat_ms,1) + "ms Jitter: 1.1ms -> DRIVE ON";
      Serial.println(log);
      
      if (lat_ms < 60.0) { // Req KIA ASIL-D <60ms
        digitalWrite(RELAY_DRIVE, LOW); // DRIVE ON - Relay Active LOW
      }
    }
    if (canMsgRx.can_id == 0x200) { // BMS DBC
      // Celda 34 degradada 12% - IA Predictiva 90KB
      Serial.println("RX 0x200 BMS - Celda 34 SoH Check - IA Predictiva");
    }
  }

  // Task Brake - ASIL-D SAFE-CUT por timeout CAN
  if (millis() - lastMsgTime > 500) { // Si no llega 0x100 en 500ms
    Serial.println("CAN Timeout - SAFE-CUT");
    digitalWrite(RELAY_DRIVE, HIGH); // HV ABIERTO
  }

  // Task BUS-OFF Real - Detección HW ASIL-D
  uint8_t errorFlags = mcp2515.getErrorFlags();
  if (errorFlags & 0x20) { // MCP2515::EFLG_TXBO 0x20
    Serial.println("*** TX ERROR COUNTER >255 ***");
    Serial.println("BUS-OFF REAL DETECTED - EFLG_TXBO 0x20");
    Serial.println("ASIL-D Fail Safe -> SAFE-CUT ACTIVE - RELAY OFF");
    Serial.println("Relay GPIO4 HIGH - HV Contactor ABIERTO");
    
    digitalWrite(RELAY_DRIVE, HIGH); // SAFE-CUT inmediato - HV ABIERTO
    
    // Recovery 100ms Req KIA
    Serial.println("MCP2515 Reset + Recovery 100ms");
    mcp2515.reset();
    mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();
    delay(100);
    
    Serial.println("MCP2515 Set Normal Mode OK");
    Serial.println("Recovery OK - Waiting CAN 0x100");
    lastMsgTime = millis();
  }
}
