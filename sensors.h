#include "mbed.h"
#include "HTU21D.h"
#include "TSL2561_I2C.h"

TSL2561_I2C tsl2561( p9, p10, 0x39);
HTU21D htu21d(p9, p10, 0x40); //Temp humid sensor || SDA, SCL
AnalogIn moistureAnalogIn1(p15);
AnalogIn moistureAnalogIn2(p16);

class THSensor
{
public :
    float temperature;
    int scale;  //0 - C, 1 - F, 2  - K
    float humidity;

    THSensor() {
        temperature=27.0;
        humidity=34;
        scale = 0; //celcius
    }

    void updateReadings() {
        switch (scale) {
            case 0:
                temperature = htu21d.sample_ctemp();
                break;
            case 1:
                temperature = htu21d.sample_ftemp();
                break;
            case 2:
                temperature = htu21d.sample_ktemp();
        }
        humidity = htu21d.sample_humid();
    }
};

class LightSensor
{
public:
    float luminosity;

    LightSensor() {
        luminosity = 0.0;
        tsl2561.enablePower();
    }
    void updateReadings() {
        luminosity=tsl2561.getLux();
    }
};


class FlowRateSensor
{
public:
    FlowRateSensor(PinName pin) : interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
        count=0;
        rate=0.0;
        calibrationFactor = 4.5;
        interrupt.rise(this, &FlowRateSensor::increment); // attach increment function of this counter instance
        ticker.attach(this, &FlowRateSensor::update, 1.0);
    }

    void increment() {
        count++;
    }

    void clear() {
        count = 0;
    }

    unsigned int read() {
        return count;
    }

    void update() {
        rate = (count) / calibrationFactor;
        this->clear();
    }
    InterruptIn interrupt;
    Ticker ticker;
    volatile unsigned int count;
    volatile float rate;
    float calibrationFactor;
};

class MoistureSensor
{
public:
    float moisture;
    int which;
    MoistureSensor(int i) {
        moisture = 0.0;
        which = i;
    }
    void updateReadings() {
        moisture=(which==1)?moistureAnalogIn1:moistureAnalogIn2;
    }
};