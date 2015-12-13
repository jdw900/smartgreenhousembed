#include "serial.h"

DigitalOut myled(LED1);

int main()
{
    pc.printf("\033[2J");
    pc.attach(&readInterrupt);
    heaterOut = 0;
    pc.printf("Started\r\n");
    wait(2);
    while(1) {
        //read data from all sensors
        thSensor->updateReadings();
        lightSensor->updateReadings();
        moistureSensor1->updateReadings();
        moistureSensor2->updateReadings();
        //compare data with thresholds and trigger actuators
        heater->updateActuator(thSensor->temperature);
        if(day) {
            light1->updateActuator(lightSensor->luminosity);
            light2->updateActuator(lightSensor->luminosity);
        } else {
            lightOut1=0;
            lightOut2=0;
            light1->on=false;
            light2->on=false;
        }
        fan->updateActuator(thSensor->humidity);
        waterPump->updateActuator(moistureSensor1->moisture, moistureSensor2->moisture);
        //send update to rPi through serial port
        NVIC_DisableIRQ(UART0_IRQn);
        int v1On = flowValve1On;
        int v1Off = flowValve1Off;
        pc.printf("Sensor:field1=%4.2f&field2=%4.2f&field3=%4.2f&field4=%4.2f&field5=%4.2f&field6=%4.2f&field7=%4.2f\r\n",thSensor->temperature, thSensor->humidity, lightSensor->luminosity, moistureSensor1->moisture, moistureSensor2->moisture, flowSensor1->rate, flowSensor2->rate);
        pc.printf("Actuator:field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d\r\n",heater->on, fan->on, light1->on, light2->on, waterPump->valve1->on, waterPump->valve2->on, waterPump->on);
        pc.printf("Threshold:field1=%4.2f&field2=%4.2f&field3=%4.2f&field4=%4.2f&field5=%4.2f\r\n",heater->threshold, fan->threshold, light1->threshold, waterPump->valve1->threshold, waterPump->valve2->threshold);
        pc.printf("\r\n");
        pc.printf("%d\r\n",i);
        NVIC_EnableIRQ(UART0_IRQn);
        wait(5);
    }
}