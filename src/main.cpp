#include <Arduino.h>
#include "esp_sleep.h"

#define us_TO_S_FACTOR 1000000ULL

RTC_DATA_ATTR int bootCount = 0;  //Declare variable in RTC Fast Memory using RTC_DATA_ATTR

int normalCount = 0;  //This ordinary variable will be RESET to 0 every time we wake up

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Day 4: RTC Memory Retention ===");
  
  bootCount++;
  normalCount++;

  //Printf values to verify
  Serial.printf("RTC Variable(bootCount) = %d <--Preserved!\n",bootCount);
  Serial.printf("Normal Variable(normalCount) = %d <--Preserved!\n",normalCount);

  Serial.println("Sleeping for 5 seconds...");
  Serial.flush();

  //Wake uo in 5 seconds to check again
  esp_sleep_enable_timer_wakeup(5*us_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop()
{
  //Empty
}