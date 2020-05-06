/*
 * adc_teensy3.cpp - Interface to configure ADC speed and capture data for Teensy 3.x boards.
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 */
#ifdef __MK20DX256__
#include "adc_teensy3.h"

// Actual voltage of internal 1.2V ref in mV. This is different from chip to chip.
// = VRefMeasured*1200/3.3 for 3.3V systems.
#define INTERNAL_REF_MV (3280*1200L/3300)

#define INTERNAL_REF_PORT 39
#define ADC_CLOCK_TO_SAMPLING 15

static unsigned int averagingTable[] = {32, 16,  8,  4,  0, 0};
static unsigned int adcBits[]        = {12, 12, 10, 10, 10, 8};

bool ADCInput::init(uint8_t newInput, uint8_t mode)
{
    input = newInput;
    pinMode(input, INPUT);
    return setMode(mode);
}

/*
 * Get the number of
 */
uint8_t ADCInput::getModeCount(){
    return sizeof(averagingTable)/sizeof(*averagingTable);
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(uint8_t mode=0){
    if (mode < ADCInput::getModeCount()){
        curMode = mode;
        bits = adcBits[mode];
        analogReadRes(bits);
        analogReadAveraging(averagingTable[mode]);
        return true;
    } else {
        return false;
    }
}

/*
 * Read internal reference voltage.
 */
uint16_t ADCInput::calibrateAREF(){
    // reset ADC to default mode for highest precision.
    uint8_t oldMode = curMode;
    setMode(0);
    analogReadRes(16);
    analogRead(INTERNAL_REF_PORT);
    delay(200);
    uint16_t rangemV = INTERNAL_REF_MV * ((1L<<16)-1) / analogRead(INTERNAL_REF_PORT);

    // set ADC to previous mode
    setMode(oldMode);
    read();

    return rangemV;
}

/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
uint32_t ADCInput::getClock(){
    return F_CPU/8;
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
uint32_t ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}

#endif /* __MK20DX256__ */
