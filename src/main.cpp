#include <Arduino.h>
#include "esp_sleep.h"

// ==========================================
// DEFINITIONS & TIMING CONFIGURATION
// ==========================================
#define REPORT_INTERVAL_SEC   30            // Data reporting interval (30 seconds)
                                            // 数据上报周期（30秒）
#define uS_TO_S_FACTOR        1000000ULL    // Microseconds to seconds factor
                                            // 微秒到秒的转换系数
#define BOOT_BUTTON_PIN       GPIO_NUM_0    // External manual override button
                                            // 外部手动干预按键

// Persistent counter to track cycles across deep sleep
// 使用 RTC 内存持久化变量，记录设备经历的“唤醒-睡眠”周期数
RTC_DATA_ATTR int cycleCount = 0;

// ==========================================
// SIMULATED HARDWARE & NETWORK SUBSYSTEMS
// 模拟的硬件与网络子系统（工业级安全关闭流程）
// ==========================================

/**
 * @brief Power up sensors, read data, and power down sensor rails immediately.
 * @brief 传感器上电、读取数据，并立即关闭传感器供电引脚以省电。
 */
void perform_sensor_telemetry() {
    Serial.println("[Sensor] Powering up sensor hardware rails...");
    delay(100); // Wait for voltage stabilization / 等待电压稳定
    
    // Simulate I2C/ADC reading / 模拟 I2C 或 ADC 采样
    float temperature = 24.0 + (rand() % 30) / 10.0;
    float humidity = 50.0 + (rand() % 20) / 10.0;
    
    Serial.printf("[Sensor] Telemetry captured: Temp=%.1f C, Hum=%.1f%%\n", temperature, humidity);
    Serial.println("[Sensor] Powering down sensor rails to prevent leakage current.");
}

/**
 * @brief Initialize Wi-Fi and MQTT connection, publish packet, and perform clean disconnect.
 * @brief 初始化 Wi-Fi 与 MQTT 连接，发布数据包，并执行安全的断开连接流程。
 */
bool execute_mqtt_publish() {
    Serial.println("[Network] Activating Wi-Fi radio and establishing RF link...");
    delay(500); // Simulate Wi-Fi connection handshake / 模拟 Wi-Fi 握手时间
    
    Serial.println("[MQTT] Connecting to cloud broker...");
    Serial.println("[MQTT] Protocol established. Publishing telemetry payload via JSON...");
    delay(200); // Simulate network latency / 模拟网络延迟
    
    Serial.println("[MQTT] Publish ACK received successfully.");
    
    // CRITICAL LOW POWER STEP: Cleanly terminate connections to avoid cloud side-effects or high current hang
    // 关键低功耗步骤：优雅地断开连接，避免云端异常或芯片网络模块死锁产生高电流
    Serial.println("[MQTT] Sending DISCONNECT packet...");
    Serial.println("[Network] Shutting down Wi-Fi RF circuitry (esp_wifi_stop)...");
    return true;
}

// ==========================================
// MAIN EXECUTION FLOW (SINGLE-SHOT ARCHITECTURE)
// 主执行流程（单次触发架构）
// ==========================================

void setup() {
    Serial.begin(115200);
    delay(1000); // Let UART stabilize / 让串口稳定
    
    cycleCount++;
    Serial.println("\n==================================================");
    Serial.printf("  ESP32-S3 EVENT-DRIVEN FIRMWARE | CYCLE: %d\n", cycleCount);
    Serial.println("==================================================");

    // 1. Check if we woke up from sleep or standard power-on
    // 1. 检查是睡眠唤醒还是常规首次上电
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("[Status] Triggered by routine hardware timer interval.");
    } else if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("[Status] Triggered by manual override (User pressed Button).");
    } else {
        Serial.println("[Status] Cold boot / System initialization.");
    }

    // 2. Execute the entire core workload sequentially
    // 2. 按顺序执行整个核心业务流水线（不走多任务空转轮询）
    perform_sensor_telemetry();
    execute_mqtt_publish();

    // 3. Prepare the chip for the next Deep Sleep cycle
    // 3. 为下一个深度睡眠周期配置硬件唤醒源（双保险：定时+外部按键）
    Serial.printf("\n[Power Management] Scheduling next wakeup in %d seconds...\n", REPORT_INTERVAL_SEC);
    esp_sleep_enable_timer_wakeup(REPORT_INTERVAL_SEC * uS_TO_S_FACTOR);
    
    // Allow user to wake up the device immediately by pressing the button before the timer expires
    // 允许用户在定时器到期前，通过按下物理按键立刻唤醒设备进行应急处理
    esp_sleep_enable_ext0_wakeup(BOOT_BUTTON_PIN, 0); 

    Serial.println("[Power Management] Flushing serial buffers and entering Deep Sleep.");
    Serial.println(">> System entering sleep. CPU and RAM powering down now. <<");
    Serial.flush();
    
    // 4. Fall asleep immediately. Execution halts here.
    // 4. 立刻进入深度睡眠。程序在此处切断电轨，停止执行。
    esp_deep_sleep_start();
    
    // This line will never be reached in an event-driven deep sleep architecture
    // 在事件驱动的深度睡眠架构中，此行代码绝对不会被触及
    Serial.println("Error: Critical leak in power management flow.");
}

void loop() {
    // CRITICAL: The infinite loop is entirely empty!
    // 关键点：传统的死循环轮询被彻底抛弃，整个函数完全留空！
    // The CPU never wastes precious battery power spinning here.
    // CPU 绝不在代码的空转中浪费宝贵的电池电量。
}
