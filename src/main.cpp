#include <Arduino.h>
#include "esp_sleep.h"

#define us_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP_SEC 10
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Day 2: Deep Sleep & Timer Wakeup ===");
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
  {
    Serial.println("[Success] Woken up by Timer!(Cold Boot-system restarted from setup)");
  }else{
    Serial.println("System started normally(Power-on Reset/EN button)");
  }
  
  Serial.printf("Preparing to enter Deep Sleep for %d seconds...\n",TIME_TO_SLEEP_SEC);
  Serial.println("Chip will power down almost entirely.Current drops to micro-amps");
  Serial.flush();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_SEC*us_TO_S_FACTOR);
  esp_deep_sleep_start();
  Serial.println("This will never print.");
}

void loop()
{
  //Empty loop
}