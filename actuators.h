#include "mbed.h"
#include "PID.h"
#define AUTO 1

DigitalOut heaterOut(p21,0);
DigitalOut flowValve1On(p11,0);
DigitalOut flowValve1Off(p12,1);
DigitalOut flowValve2On(p23,0);
DigitalOut flowValve2Off(p24,1);
DigitalOut lightOut1(p27,0);
DigitalOut lightOut2(p30,0);
PID controller(1.0, 0.1, 0.0, 5.0);        //Kc, Ti, Td, interval
PwmOut pump(p25);
DigitalOut fanOut(p28,0);

class Heater
{
public:
    float threshold;
    bool on;

    Heater() {
        threshold = 20.0;
        on = false;
    }
    void updateActuator(float temperature) {
        if(!on && temperature < threshold) {
            heaterOut = 1;
            on = true;
        } else if(on && temperature > threshold+1) {
            heaterOut = 0;
            on = false;
        }
    }

};


class FlowValve
{
public:
    int which;
    float threshold;
    bool on;

    FlowValve(int i) {
        which=i;
        threshold = 0.5;
        on = false;
    }
    void updateActuator(float moisture) {
        if(moisture < threshold) {
            switch(which) {
                case 1:
                    flowValve1On=1;
                    flowValve1Off=0;
                    break;
                case 2:
                    flowValve2On=1;
                    flowValve2Off=0;
                    break;
            }
            on = true;
        } else if(moisture > threshold) {
            switch(which) {
                case 1:
                    flowValve1On=0;
                    flowValve1Off=1;
                    break;
                case 2:
                    flowValve2On=0;
                    flowValve2Off=1;
                    break;
            }
            on = false;
        }
    }
};


class Light
{
public:
    int which;
    float threshold;
    bool on;

    Light(int i) {
        which=i;
        threshold = 10.0;
        on = false;
    }
    void updateActuator(float luminosity) {
        if(!on && luminosity < threshold) {
            switch(which) {
                case 1:
                    lightOut1=1;
                    break;
                case 2:
                    lightOut2=1;
                    break;
            }
            on = true;
        } else if(on && luminosity > threshold) {
            switch(which) {
                case 1:
                    lightOut1=0;
                    break;
                case 2:
                    lightOut2=0;
                    break;
            }
            on = false;
        }
    }
};

class WaterPump
{
public:
    FlowValve* valve1;
    FlowValve* valve2;
    bool on;
    WaterPump() {
        valve1 = new FlowValve(1);
        valve2 = new FlowValve(2);
        on=false;
        //Analog input from 0.0 to 3.3V
        controller.setInputLimits(0.0, 1.0);
        //Pwm output from 0.0 to 1.0
        controller.setOutputLimits(0.0, 1.0);
        //If there's a bias.
        controller.setBias(0.3);
        controller.setMode(AUTO);
    }
    void updateActuator(float moisture1, float moisture2) {
        valve1->updateActuator(moisture1);
        valve2->updateActuator(moisture2);

        bool anyOn = valve1->on || valve2->on;
        float threshold;
        float input;
        if(anyOn) {
            if(valve1->on && valve2->on) {
                threshold = (valve1->threshold<valve2->threshold)?valve1->threshold:valve2->threshold;
                input = (valve1->threshold<valve2->threshold)?moisture1:moisture2;
            } else if(valve1->on) {
                threshold = valve1->threshold;
                input = moisture1;
            } else {
                threshold = valve2->threshold;
                input = moisture2;
            }
        } else {
            threshold = (valve1->threshold<valve2->threshold)?valve1->threshold:valve2->threshold;
            input = (valve1->threshold<valve2->threshold)?moisture1:moisture2;
        }
        controller.setSetPoint(threshold);
        controller.setProcessValue(input);
        if(anyOn) {
            pump=controller.compute();
            on=true;
        } else {
            pump=0;
            on=false;
        }
    }

};

class Fan
{
public:
    float threshold;
    bool on;

    Fan() {
        threshold = 50.0;
        on = false;
    }
    void updateActuator(float humidity) {
        if(!on && humidity > threshold) {
            fanOut = 1;
            on = true;
        } else if(on && humidity < threshold+1) {
            fanOut = 0;
            on = false;
        }
    }

};