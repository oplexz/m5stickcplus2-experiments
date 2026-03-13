#ifndef BLE_TEMP_SENSOR_H
#define BLE_TEMP_SENSOR_H

#include <Arduino.h>
#include <M5GFX.h>
#include <NimBLEDevice.h>

class BleTempSensor
{
  public:
    BleTempSensor(M5GFX& screen);

    void setTargetName(const char* name);

    void start();

    bool tick();

    void stop();

    bool isConnected() const;

    float temperature() const;
    float humidity() const;
    uint8_t batteryLevel() const;

  private:
    enum class State
    {
        Idle,
        Scanning,
        Connecting,
        Reading,
        Displaying,
        Error,
    };

    void drawStatus(const char* line1, const char* line2 = nullptr);
    void drawReadings();
    bool connectToDevice();
    bool readValues();

    M5GFX& screen_;
    NimBLEClient* client_;

    const char* targetName_;

    State state_;
    uint32_t lastReadMs_;
    uint32_t readIntervalMs_;
    uint32_t stateEnteredMs_;

    float temperature_;
    float humidity_;
    uint8_t batteryLevel_;
    bool hasData_;

    NimBLEAddress foundAddress_;
    bool addressFound_;

    static constexpr uint32_t kScanTimeoutMs = 10000;
    static constexpr uint32_t kDefaultReadIntervalMs = 10000;

    static constexpr uint16_t kEnvSensingService = 0x181A;
    static constexpr uint16_t kTempChar = 0x2A6E;
    static constexpr uint16_t kHumidityChar = 0x2A6F;
    static constexpr uint16_t kBatteryService = 0x180F;
    static constexpr uint16_t kBatteryChar = 0x2A19;
};

#endif
