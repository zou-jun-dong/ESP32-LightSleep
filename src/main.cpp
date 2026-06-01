#include <Arduino.h>
#include "esp_sleep.h"

#define us_TO_S_FACTOR 1000000ULL
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Day 1: Light Sleep Test ===");
}

void loop()
{
  Serial.println("System is running normal active tasks...");
  delay(1000);

  Serial.println("Entering Light Sleep for 5 seconds...(Peripherals & retained)");
  Serial.flush();  //Ensure UART buffer is empty before sleeping

  //Configure Timer for Light Sleep
  esp_sleep_enable_timer_wakeup(5*us_TO_S_FACTOR);
  //Start Light sleep.CPU pauses,but RAM is kept alive
  esp_light_sleep_start();

  //[Verification] if it is a Warm Boot,execution continues right here
  Serial.println("Woken up from Light Sleep! (Warm Boot Success)");
  Serial.println("Notice that setup() was NOT caller again.\n");

  delay(2000);
}