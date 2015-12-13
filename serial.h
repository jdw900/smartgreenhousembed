#include "sensors.h"
#include "actuators.h"
#include <string>
#include <sstream>

using namespace std;
string serialInput;
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led(LED1);
DigitalOut led2(LED4);
DigitalOut led3(LED3, 0);
int i=0;
bool day = true;

//sensors
THSensor* thSensor = new THSensor();
FlowRateSensor* flowSensor1 = new FlowRateSensor(p5);
FlowRateSensor* flowSensor2 = new FlowRateSensor(p6);
LightSensor* lightSensor = new LightSensor();
MoistureSensor* moistureSensor1 = new MoistureSensor(1);
MoistureSensor* moistureSensor2 = new MoistureSensor(2);

//actuators
Heater* heater = new Heater();
WaterPump* waterPump = new WaterPump();
Light* light1 = new Light(1);
Light* light2 = new Light(2);
Fan* fan = new Fan();

void processInput(string input)
{
    stringstream allFieldsStream(input);
    string item;
    while (getline(allFieldsStream, item, ';')) {
        stringstream fieldStream(item);
        string type;
        string value;
        if(getline(fieldStream, type, '=')) {
            if(getline(fieldStream, value, '=')) {
                if(type.compare("h")==0) {
                    fan->threshold = atof(value.c_str());
                } else if(type.compare("t")==0) {
                    heater->threshold = atof(value.c_str());
                } else if(type.compare("l")==0) {
                    light1->threshold = atof(value.c_str());
                    light2->threshold = atof(value.c_str());
                } else if(type.compare("sm1")==0) {
                    waterPump->valve1->threshold = atof(value.c_str());
                } else if(type.compare("sm2")==0) {
                    waterPump->valve2->threshold = atof(value.c_str());
                } else if(type.compare("d")==0) {
                    if(value.compare("true")==0) {
                        day=true;
                    } else {
                        day=false;
                    }
                }
            }
        }
    }
}

void readInterrupt(void)
{
    i++;
    NVIC_DisableIRQ(UART0_IRQn);
    led=1;
    //buffer variables
    char ch;
    //if data is ready in the buffer
    if (pc.readable()) {
        led3=!led3;
        //read 1 character
        ch = pc.getc();
        //pc.putc(ch);
        //if character is $ than it is the start of a sentence
        if (ch == '$') {
            //so the pointer should be set to the first position
            serialInput.clear();
        }
        //if the character is # than the end of the sentence
        else if (ch == '#') {
            //do something with the string
            //pc.printf("%s\r\n",serialInput.c_str());
            led2=1;
            processInput(serialInput);
            led2=0;
        } else {
            //write buffer character to big buffer string
            serialInput += ch;
        }
    }
    led=0;
    NVIC_EnableIRQ(UART0_IRQn);
}