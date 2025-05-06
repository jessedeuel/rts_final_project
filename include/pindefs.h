#ifndef PINDEFS_H
#define PINDEFS_H

#include <pico/stdlib.h>


/*####################################
         Digital IO Pins
####################################*/

//OUTPUTS
#define PROBE_PIN  8
//FOR TESTING USE pin 16. Default is 4.
#define NEOPIXEL_DATA_PIN  4

#define HEARTBEAT  LED_PIN

#define LOAD_1_SHUNT  19
#define LOAD_2_SHUNT  20

#define LED_PIN  PICO_DEFAULT_LED_PIN


//For testing use pin 4 default pin 16
#define Z_INV  16
#define PULL_INV  17
#define IDX_INV  18

#define ADC_SELECT  21

//INPUTS

#define Z_MONITOR  3
#define PULL_MONITOR  9
#define IDX_MONITOR  2
#define INDUCT_MONITOR  5
/*#################################
##################################*/

/*####################################
            SPI SIGNALS
####################################*/

#define SPI_MOSI  12
#define SPI_MISO  11
#define SPI_CLK  10
#define I_SENSE_1_CS  13
#define I_SENSE_2_CS  14
/*#################################
##################################*/

/*####################################
            ADC SIGNALS
####################################*/
#define ADC0  26
#define ADC1  27
#define ADC2  28
#define ADC3  29
/*#################################
##################################*/


/*####################################
            UART0 SIGNALS
####################################*/
#define TX_PIN  0
#define RX_PIN  1
/*#################################
##################################*/



/*####################################
           I2C SIGNALS
####################################*/
#define I2C_SDA  6
#define I2C_SCL  7
/*#################################
##################################*/

#endif