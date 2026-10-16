# MyAutoOS M-Shield - VCU Engine Mexicano | ASIL-D Ready
### MyAutoOS Technologies - El Cerebro de México para EV 🇲🇽🧠
**Founder: Norberto - ITSU Uruapan, Michoacan| Proveedor Tier2/Tier1 KIA Pesquería / BMW SLP / BYD Lerma**

> México no importa cerebros, los fabrica. VCU completo $645 MXN BOM vs $2,500 USD Bosch/Continental. PoC SiL Validado LAT 53.2ms.

---

## 🧠 VCU COMPLETO - TODO LO DEL CEREBRO

| Sistema | Función | Validación |
|---------|---------|------------|
| **Batería BMS** | SoC 75%, SoH, Celda 34 degradada 12% | IA Predictiva 90KB TFLite Micro |
| **Freno Brake** | ASIL-D SAFE-CUT | 53.2ms <60ms KIA Req |
| **Velocidad** | CAN 0x100 / 0x200 Torque Control | 500KBPS HEV/EV |
| **HV Contactor** | ABIERTO/CERRADO | GPIO4 Relay Drive |
| **OTA** | MQTT TLS 1.3 | Gemelo Digital Wokwi |
| **Nube** | AWS IoT | Dashboard SDV Python |

---

## ⚡ PERFORMANCE VALIDADO SiL WOKWI

- **LAT:** 53.2ms (Req KIA <60ms) - Jitter 1.1ms - **ASIL-D OK**
- **BUS-OFF:** Real HW Detection EFLG_TXBO 0x20 + Recovery 100ms - Req KIA ASIL-D
- **BOM:** $645 MXN | **Low Power:** <5mA Sleep | **CAN:** 500KBPS ID 0x100
- **Repo:** Presentación Presencial Tier1 - MyAutoOS Technologies

### Evidencia SiL:
- Latencia: `evidence/LAT_53.2ms_ASIL-D.jpg`
- BUS-OFF Real: `evidence/BUS-OFF_Real_ASIL-D.jpg` - TX Error >255 -> SAFE-CUT -> Reset 100ms -> Recovery

---

## 📊 PITCH TIER1 - 3 OEMs Que Pagan $2M

![Pitch Tier1](pitch/PITCH_TIER1_BMW_KIA_BYD.png)

**BMW SLP Neue Klasse EV 2026:** Dolor $2M migración Vector->Aurix TC3xx | Solución MyAutoOS AUTOSAR Port Aurix Ready 53ms

**KIA Pesquería EV3 400V/800V 58kWh:** Dolor $50k validación BMS Kalman + HIL Continental | Solución MyAutoOS DBC 0x200 BMS Cell 34 degradada 12% predicción PoC 2 semanas 90KB

**BYD LERMA Dolphin Mini:** Dolor OTA insegura | Solución MyAutoOS OTA MQTT TLS 1.3 + Gemelo Wokwi + Dashboard SDV 53ms

---

## 🏗️ ARQUITECTURA GEMELO DIGITAL SDV 2026

![SDV Architecture](pitch/GEMELO_DIGITAL_SDV_2026.png)

- **HW:** ESP32-S3 EYE + OV2640 + BMS + MCP2515 TJA1050
- **Gemelo:** Wokwi + QEMU + AUTOSAR 4 Capas + FreeRTOS + IA Embebida TFLite Micro
- **Dashboard SDV:** SoC 75% Temp 45°C Latencia 53ms HV ABIERTO/CERRADO
- **IA Predictiva:** Celda 34 degradada 12% - Programar mantto
- **Conectividad:** ESP32 -> Cloud AWS IoT / MQTT / TLS 1.3 + OTA Update por IA

---

## 🔌 WIRING DIAGRAM CORREGIDO ASIL-D

![Wiring Corrected](datasheet/wiring_diagram_CORRECTED_ASIL-D.png)

**Corregido Tier2 KIA:**
- SPI: GPIO5->CS Yellow, GPIO18->SCK Orange, GPIO19->MISO Green, GPIO23->MOSI Blue
- Power: ESP32 3.3V -> Red Rail -> VCC MCP2515 & Relay (Red), GND -> Blue Rail -> GND All (Black)
- Relay: GPIO4 -> IN Purple | Active LOW
- CAN BUS: CANH CANL GND Correcto

**Notas:** All modules common GND | MCP2515 SPI | Relay Active LOW | Power ESP32 USB 5V 3.3V rail powers modules

---

## 💻 CÓDIGO VCU ASIL-D

```cpp
// BUS-OFF Real HW Detection ASIL-D
if (mcp2515.getErrorFlags() & 0x20) { // EFLG_TXBO
  Serial.println("BUS-OFF REAL - SAFE-CUT");
  digitalWrite(RELAY_DRIVE, LOW); // HV ABIERTO
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  delay(100); // Recovery KIA 100ms
  }
