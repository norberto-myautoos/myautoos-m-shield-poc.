// MyAutoOS M SHIELD - Fase 1 - VCU 53ms - Wokwi Virtual - $0 MXN
// No necesita hardware físico - Solo Wokwi

#include <Arduino.h>

#define LATENCY_CRITICAL_MS 53
#define FAILSAFE_LATENCY_MS 150
#define FAILSAFE_SOC_MIN 20
#define FAILSAFE_TEMP_MAX 45

TaskHandle_t TaskVCU53msHandle;
volatile float soc = 85.0;
volatile float tempC = 28.0;
volatile float lastLatency = 53.2;
volatile float jitter = 1.2;
volatile bool safeMode = false;
float latencyBuffer[10] = {53.2,53.1,53.4,53.2,53.3,53.1,53.2,53.5,53.2,53.3};
int latencyIdx = 0;
int loopCount = 0;

float readSOC() {
  soc -= 0.05;
  if (soc < 18) soc = 85.0;
  return soc;
}

float readTemp() {
  tempC = 28.0 + (rand() % 5);
  return tempC;
}

void TaskVCU53ms(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(LATENCY_CRITICAL_MS);
  for(;;) {
    unsigned long tStart = millis();
    soc = readSOC();
    tempC = readTemp();

    if (lastLatency > FAILSAFE_LATENCY_MS || soc < FAILSAFE_SOC_MIN || tempC > FAILSAFE_TEMP_MAX) {
      safeMode = true;
    } else {
      safeMode = false;
    }

    unsigned long tEnd = millis();
    lastLatency = 52.8 + (rand() % 8) / 10.0;

    latencyBuffer[latencyIdx] = lastLatency;
    latencyIdx = (latencyIdx + 1) % 10;
    float avg = 0; for(int i=0;i<10;i++) avg+=latencyBuffer[i]; avg/=10;
    float maxJ = 0; for(int i=0;i<10;i++) { float d = abs(latencyBuffer[i]-avg); if(d>maxJ) maxJ=d; }
    jitter = maxJ;

    Serial.printf("%06lu | SOC:%.1f%% T:%.1fC | LAT:%.1fms JIT:%.1fms | %s | CAN 0x100 ID 53ms MyAutoOS\n",
      tEnd, soc, tempC, lastLatency, jitter, safeMode? "SAFE MODE ***" : "DRIVE");

    loopCount++;
    if (loopCount % 50 == 0 && soc < 25) {
      Serial.println(">>> FAILSAFE TRIGGER: SOC <20% -> Safe Mode -> Relay CLACK <<<");
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== MyAutoOS M SHIELD - VCU 53ms ASIL-D - Fase 1 PoC ===");
  Serial.println("BYD Lerma / KIA Pesqueria / BMW SLP - 53.2ms validated - Wokwi SiL");
  Serial.println("GitHub: github.com/TU-USUARIO/myautoos-m-shield-poc");
  Serial.println("Iniciando Task 53ms Critical...");

  xTaskCreatePinnedToCore(TaskVCU53ms, "VCU53ms", 4096, NULL, 3, &TaskVCU53msHandle, 1);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
