#include <Arduino.h>
#include "esp_sleep.h"

#define BUTTON_PIN GPIO_NUM_0
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Day 3: External Ext0 Wakeup ===");

  //Check if woken up by external button
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
  {
    Serial.println("[Success] Instant recovery!Wake up by Physical Button(GPIO 0)");
  }else{
    Serial.println("Normal system boot up");
  }
  
  Serial.println("Configuring Ext0 Wakeup on GPIO 0...");
  //Enable Ext0 Wakeup.
  esp_sleep_enable_ext0_wakeup(BUTTON_PIN,0);

  Serial.println("Entering Deep Sleep now.press the BOOT button to wake me up");
  Serial.flush();

  //Enter Deep Sleep
  esp_deep_sleep_start();
}

void loop()
{
  //Empty loop
}