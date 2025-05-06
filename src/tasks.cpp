#include <stdio.h>
#include <array>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"

#include "pindefs.h"
#include "tasks.h"
#include "PicoLED/PicoLed.hpp"
#include "cal_funcs.h"

extern int8_t data;
extern std::array<uint8_t, 5> neopixel_data;
extern PicoLed::Color active_color;
extern PicoLed::PicoLedController ledStrip;

//int32_t ReadUART(uart_inst_t *uart, uint8_t *const buff, int32_t const buff_size);
//ProtocolCmd IdentifyBuffFormat(uint8_t *const buff);


void process_commands()
{
    //printf("In process_commands");
    if(stdio_usb_connected())
    {
        data = stdio_getchar_timeout_us(50);
    }
}

void read_loadcells()
{
    //printf("In read_loadcells");
    if (data == 'c')
    {
        gpio_put(ADC_SELECT, true);
        adc_select_input(1);
        float load1_value = (adc_read()/4095.0) * 3.3;
        adc_select_input(2);
        float load2_value = (adc_read()/4095.0) * 3.3;
        printf("Load 1: %f, Load 2: %f\n", load1_value, load2_value);
    }
}

void update_light()
{
    if (data == 'l')
    {
    //printf("In update_light");
        for (int i = 0; i < 5; i++)
        {
            // Packet in format: Brightness | Red | Green | Blue | White
            neopixel_data[i] = (uint8_t)stdio_getchar_timeout_us(50);
        }

        for (int i = 0; i < 5; i++)
        {
            printf("[%d]: %d\n", i, neopixel_data[i]);
        }

        active_color.red = neopixel_data[1];
        active_color.green = neopixel_data[2];
        active_color.blue = neopixel_data[3];
        active_color.white = neopixel_data[4];

        ledStrip.setBrightness(neopixel_data[0]);
        ledStrip.fill(active_color);
        ledStrip.show();
    }
}

void read_motor_current()
{

    if (data == 'm')
    {
        std::array<uint16_t, 6> output = {};

        printf("Motor Current: \n");

        for (int i = 0; i < 6; i++) {
            uint16_t current = GetMotorCurrent(i);
            output[i] = current;            
            printf("[%d]: %d\n", i, current);
        }
    }
}
