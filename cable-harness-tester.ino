#include "Arduino.h"

typedef struct
{
    bool skip;
    byte connectedTo;
}TESTPIN;
const byte LISTLENGTH=8;

TESTPIN learnlist[LISTLENGTH];
TESTPIN testlist[LISTLENGTH];

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Cable tester");
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);
    learnlist[0].skip=true;//serial port RX0, 0 must always be skipped, as it is the default value of the sampleList items.
    learnlist[1].skip=true;//serial port TX0
    learnlist[13].skip=true;
    for(byte i=1;i<LISTLENGTH;i++)
    {
        testlist[i].skip=learnlist[i].skip;
    }
}

// the loop function runs over and over again forever
void loop()
{
    while (Serial.available())
    {
        // get the new byte:
        switch(Serial.read())
        {
        case 't'://test
            measureList(testlist, LISTLENGTH);
            compareList(learnlist, testlist);
            //printList(testlist);
            break;
        case 'l'://learn
            measureList(learnlist, LISTLENGTH);
            //printList(learnlist);
            break;
        }
    }
}

bool compareList(TESTPIN refList[], TESTPIN sampleList[])
{
    char str[40];
    bool ret=true;
    for(byte i=1;i<LISTLENGTH;i++)
    {
        if(refList[i].skip)
        {
            continue;
        }
        if(refList[i].connectedTo != sampleList[i].connectedTo)
        {
            if(i==sampleList[i].connectedTo)
            {
                sprintf(str,"Error: pin %d has no connection to %d", refList[i].connectedTo, i);
            }else
            {
                sprintf(str,"Error: pin %d connected to pin %d", sampleList[i].connectedTo, i);
            }
            Serial.println(str);
            ret=false;
        }
    }
    if(ret)
    {
        Serial.println("All ok");
    }
    return ret;
}

void printList(TESTPIN pinlist[])
{
    char str[40];
    for(byte i=1;i<LISTLENGTH;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        sprintf(str,"%d : %d", i, pinlist[i].connectedTo);
        Serial.println(str);
    }
}

void measureList(TESTPIN pinlist[], byte length)
{
    //Set all pins to be tested to input pull-up and remove existing connections.
    for(byte i=1;i<length;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        pinMode(i,INPUT_PULLUP);
        pinlist[i].connectedTo=0;
    }
    //Loop through all the pins and find the connections
    for(byte i=1;i<length;i++)
    {
        if(pinlist[i].skip)
        {
            continue;
        }
        pinMode(i, OUTPUT);
        digitalWrite(i,0);
        for(byte j=i+1;j<length;j++)
        {
            if(pinlist[j].skip || pinlist[j].connectedTo)
            {
                continue;
            }
            if(!digitalRead(j))
            {
                pinlist[j].connectedTo=i;
            }
        }
        if(!pinlist[i].connectedTo)
        {
            pinlist[i].connectedTo=i;
        }
        pinMode(i, INPUT);
    }

}
