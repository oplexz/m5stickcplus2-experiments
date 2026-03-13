#include "BleTempSensor.h"

BleTempSensor::BleTempSensor(M5GFX& screen)
: screen_(screen)
, client_(nullptr)
, targetName_("ATC_1069C9")
, state_(State::Idle)
, lastReadMs_(0)
, readIntervalMs_(kDefaultReadIntervalMs)
, stateEnteredMs_(0)
, temperature_(0)
, humidity_(0)
, batteryLevel_(0)
, hasData_(false)
, addressFound_(false)
{
}

void BleTempSensor::setTargetName(const char* name)
{
    targetName_ = name;
}

void BleTempSensor::start()
{
    hasData_ = false;
    addressFound_ = false;
    state_ = State::Scanning;
    stateEnteredMs_ = millis();

    NimBLEDevice::init("");
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    scan->start(0, false); // continuous scan

    drawStatus("Scanning for", targetName_);
}

bool BleTempSensor::tick()
{
    uint32_t now = millis();

    switch (state_)
    {
    case State::Idle:
        return false;

    case State::Scanning:
    {
        NimBLEScan* scan = NimBLEDevice::getScan();
        NimBLEScanResults results = scan->getResults();

        for (size_t i = 0; i < results.getCount(); ++i)
        {
            const NimBLEAdvertisedDevice* dev = results.getDevice(i);
            if (dev->haveName() && dev->getName() == targetName_)
            {
                foundAddress_ = dev->getAddress();
                addressFound_ = true;
                scan->stop();
                scan->clearResults();

                state_ = State::Connecting;
                stateEnteredMs_ = now;
                // Only show status text on first connect
                if (!hasData_)
                {
                    drawStatus("Found!", "Connecting...");
                }
                return true;
            }
        }

        // Timeout
        if (now - stateEnteredMs_ > kScanTimeoutMs)
        {
            scan->stop();
            scan->clearResults();

            if (hasData_)
            {
                // Already have readings, just stay on display and retry later
                state_ = State::Displaying;
                stateEnteredMs_ = now;
            }
            else
            {
                state_ = State::Error;
                stateEnteredMs_ = now;
                drawStatus("Not found", targetName_);
            }
            return true;
        }
        return false;
    }

    case State::Connecting:
    {
        if (connectToDevice())
        {
            state_ = State::Reading;
            stateEnteredMs_ = now;
        }
        else
        {
            if (hasData_)
            {
                // Silently go back to display, retry later
                state_ = State::Displaying;
                stateEnteredMs_ = now;
            }
            else
            {
                state_ = State::Error;
                stateEnteredMs_ = now;
                drawStatus("Connect failed", "Will retry...");
            }
        }
        return !hasData_;
    }

    case State::Reading:
    {
        bool ok = readValues();
        // Disconnect regardless to save power on both ends
        if (client_ && client_->isConnected())
        {
            client_->disconnect();
        }

        if (ok)
        {
            hasData_ = true;
            lastReadMs_ = now;
            drawReadings();
            state_ = State::Displaying;
            stateEnteredMs_ = now;
            return true;
        }
        else
        {
            if (hasData_)
            {
                state_ = State::Displaying;
                stateEnteredMs_ = now;
                return false;
            }
            else
            {
                state_ = State::Error;
                stateEnteredMs_ = now;
                drawStatus("Read failed", "Will retry...");
                return true;
            }
        }
    }

    case State::Displaying:
    {
        if (now - lastReadMs_ >= readIntervalMs_)
        {
            if (addressFound_)
            {
                state_ = State::Connecting;
                stateEnteredMs_ = now;
            }
            else
            {
                state_ = State::Scanning;
                stateEnteredMs_ = now;
                NimBLEScan* scan = NimBLEDevice::getScan();
                scan->start(0, false);
            }
        }
        return false;
    }

    case State::Error:
    {
        // Retry after 3 seconds
        if (now - stateEnteredMs_ > 3000)
        {
            state_ = State::Scanning;
            stateEnteredMs_ = now;

            NimBLEScan* scan = NimBLEDevice::getScan();
            scan->start(0, false);
            if (!hasData_)
            {
                drawStatus("Scanning for", targetName_);
            }
            return !hasData_;
        }
        return false;
    }
    }

    return false;
}

void BleTempSensor::stop()
{
    NimBLEScan* scan = NimBLEDevice::getScan();
    if (scan->isScanning())
    {
        scan->stop();
    }
    if (client_ && client_->isConnected())
    {
        client_->disconnect();
    }
    if (client_)
    {
        NimBLEDevice::deleteClient(client_);
        client_ = nullptr;
    }
    state_ = State::Idle;
}

bool BleTempSensor::isConnected() const
{
    return client_ && client_->isConnected();
}

float BleTempSensor::temperature() const { return temperature_; }
float BleTempSensor::humidity() const { return humidity_; }
uint8_t BleTempSensor::batteryLevel() const { return batteryLevel_; }

bool BleTempSensor::connectToDevice()
{
    if (!addressFound_)
    {
        return false;
    }

    if (!client_)
    {
        client_ = NimBLEDevice::createClient();
    }

    if (!client_->connect(foundAddress_))
    {
        return false;
    }

    return true;
}

bool BleTempSensor::readValues()
{
    if (!client_ || !client_->isConnected())
    {
        return false;
    }

    bool gotTemp = false;
    bool gotHum = false;

    // Read temperature from Environmental Sensing service
    NimBLERemoteService* envSvc = client_->getService(NimBLEUUID(kEnvSensingService));
    if (envSvc)
    {
        // Temperature (0x2A6E): sint16, 0.01 °C
        NimBLERemoteCharacteristic* tempChr = envSvc->getCharacteristic(NimBLEUUID(kTempChar));
        if (tempChr && tempChr->canRead())
        {
            NimBLEAttValue val = tempChr->readValue();
            if (val.length() >= 2)
            {
                int16_t raw;
                memcpy(&raw, val.data(), sizeof(raw));
                temperature_ = raw / 100.0f;
                gotTemp = true;
            }
        }

        // Humidity (0x2A6F): uint16, 0.01 %
        NimBLERemoteCharacteristic* humChr = envSvc->getCharacteristic(NimBLEUUID(kHumidityChar));
        if (humChr && humChr->canRead())
        {
            NimBLEAttValue val = humChr->readValue();
            if (val.length() >= 2)
            {
                uint16_t raw;
                memcpy(&raw, val.data(), sizeof(raw));
                humidity_ = raw / 100.0f;
                gotHum = true;
            }
        }
    }

    // Read battery level
    NimBLERemoteService* batSvc = client_->getService(NimBLEUUID(kBatteryService));
    if (batSvc)
    {
        NimBLERemoteCharacteristic* batChr = batSvc->getCharacteristic(NimBLEUUID(kBatteryChar));
        if (batChr && batChr->canRead())
        {
            NimBLEAttValue val = batChr->readValue();
            if (val.length() >= 1)
            {
                batteryLevel_ = val.data()[0];
            }
        }
    }

    return gotTemp || gotHum;
}

void BleTempSensor::drawStatus(const char* line1, const char* line2)
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextSize(2);
    screen_.setTextColor(TFT_YELLOW, TFT_BLACK);
    screen_.setCursor(8, 30);
    screen_.print(line1);
    if (line2)
    {
        screen_.setCursor(8, 55);
        screen_.setTextColor(TFT_WHITE, TFT_BLACK);
        screen_.print(line2);
    }

    screen_.setTextSize(1);
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 126);
    screen_.print("Hold Select = back");
}

void BleTempSensor::drawReadings()
{
    screen_.fillScreen(TFT_BLACK);

    // Temperature -- big and centered
    screen_.setTextSize(3);
    screen_.setTextColor(TFT_WHITE, TFT_BLACK);
    screen_.setCursor(8, 10);
    screen_.printf("%.1f", temperature_);
    screen_.setTextSize(2);
    screen_.print(" C");

    // Humidity
    screen_.setTextSize(3);
    screen_.setTextColor(TFT_CYAN, TFT_BLACK);
    screen_.setCursor(8, 50);
    screen_.printf("%.1f", humidity_);
    screen_.setTextSize(2);
    screen_.print(" %");

    // Battery
    screen_.setTextSize(1);
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 95);
    screen_.printf("Bat: %d%%  %s", batteryLevel_, targetName_);

    // Age of reading
    uint32_t ageSec = (millis() - lastReadMs_) / 1000;
    screen_.setCursor(8, 110);
    if (ageSec < 5)
    {
        screen_.setTextColor(TFT_GREEN, TFT_BLACK);
        screen_.print("Updated just now");
    }
    else
    {
        screen_.printf("Updated %lus ago", ageSec);
    }

    screen_.setCursor(8, 126);
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.print("Hold Select = back");
}
