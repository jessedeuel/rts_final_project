#ifndef CAL_FNS_H
#define CAL_FNS_H

#include "pindefs.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

//Really is 7.324mA/count but due to errors keep it simple
constexpr float ACS711_CONV_FACTOR = 7.0;              //mA/count
constexpr int32_t ACS711_CENTER_POINT = 2048;
constexpr uint8_t CURRENT_AVERAGING = 5;
constexpr uint8_t ADC_MAX_SAMPLES = 25;

//Function to get the current of A or B pole of the motor
//  Enable CS pin
//  Send measurement commands over SPI
//  Record measured current
uint16_t GetMotorCurrent(const uint8_t pole);


//ADC SPI function
uint16_t SPI_ReadADC(const uint8_t channel, spi_inst_t* spi);

//Function to verify switches are working
//  Invert switches
//  Verify the monitor is high
//  Disable inversion

//Requires Machine is not in home position
uint8_t CheckLimSw(const uint8_t sw_time = 50);


#endif