#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "lib/PicoLED/PicoLed.hpp"

#include "tasks.h"
#include "scheduler.h"
#include "pindefs.h"

#include <stdio.h>
#include <array>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// Function Prototypes
bool HeartbeatCallback(repeating_timer_t *rt);
void init();

// LED initialization
PicoLed::PicoLedController ledstrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 
    0, 
    NEOPIXEL_DATA_PIN, 
    16,
    PicoLed::FORMAT_GRB);


// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Machine specific variables
std::array<uint8_t, 5> neopixel_data = {};
PicoLed::Color active_color = PicoLed::RGBW(10,30,40,0);
PicoLed::PicoLedController ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 
    0, 
    NEOPIXEL_DATA_PIN, 
    16,
    PicoLed::FORMAT_GRB);


repeating_timer_t heartbeat_timer;
int8_t data = 0;



int main()
{
    //stdio_init_all();
    init();
    
    add_repeating_timer_ms(-250, HeartbeatCallback, NULL, &heartbeat_timer);

    // Scheduler setup
    scheduler sched(10);
    sched.add_task(process_commands);
    sched.add_task(read_loadcells);
    sched.add_task(update_light);
    sched.add_task(read_motor_current);

    while (true) {
        sched.scheduler_loop();
        //printf("Hello world!\n");
        //sleep_ms(1000);
    }
}

void init()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(PROBE_PIN);
    gpio_set_dir(PROBE_PIN, GPIO_OUT);
    gpio_pull_down(PROBE_PIN);

    gpio_init(HEARTBEAT);
    gpio_set_dir(HEARTBEAT, GPIO_OUT);
    //gpio_pull_down(PROBE_PIN);

    //OUTPUT PINS
    gpio_init(LOAD_1_SHUNT);
    gpio_set_dir(LOAD_1_SHUNT, GPIO_OUT);
    
    gpio_init(LOAD_2_SHUNT);
    gpio_set_dir(LOAD_2_SHUNT, GPIO_OUT);
    
    gpio_init(Z_INV);
    gpio_set_dir(Z_INV, GPIO_OUT);
    
    gpio_init(PULL_INV);
    gpio_set_dir(PULL_INV, GPIO_OUT);
    
    gpio_init(IDX_INV);
    gpio_set_dir(IDX_INV, GPIO_OUT);
    
    gpio_init(ADC_SELECT);
    gpio_set_dir(ADC_SELECT, GPIO_OUT);
    gpio_put(ADC_SELECT, true);

    //INPUT PINS
    gpio_init(Z_MONITOR);
    gpio_set_dir(Z_MONITOR, GPIO_IN);
    
    gpio_init(PULL_MONITOR);
    gpio_set_dir(PULL_MONITOR, GPIO_IN);

    gpio_init(IDX_MONITOR);
    gpio_set_dir(IDX_MONITOR, GPIO_IN);

    gpio_init(INDUCT_MONITOR);
    gpio_set_dir(INDUCT_MONITOR, GPIO_IN);

    //setup_uart0();

    // Setup ADC
    // ADC is in an unknown state. We should start by resetting it
    reset_block(RESETS_RESET_ADC_BITS);
    unreset_block_wait(RESETS_RESET_ADC_BITS);

    // Now turn it back on. Staging of clock etc is handled internally
    adc_hw->cs = ADC_CS_EN_BITS;

    // Internal staging completes in a few cycles, but poll to be sure
    while (!(adc_hw->cs & ADC_CS_READY_BITS)) {
        tight_loop_contents();
    }
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(ADC0);
    adc_gpio_init(ADC1);
    adc_gpio_init(ADC2);
    adc_gpio_init(ADC3);

    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // Setup SPI1
    //Hardware specific
    gpio_set_function(SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI, GPIO_FUNC_SPI);
    

    //Chip select pins
    gpio_init(I_SENSE_1_CS);
    gpio_set_dir(I_SENSE_1_CS, GPIO_OUT);
    gpio_pull_up(I_SENSE_1_CS);

    gpio_init(I_SENSE_2_CS);
    gpio_set_dir(I_SENSE_2_CS, GPIO_OUT);
    gpio_pull_up(I_SENSE_2_CS);

    //MIN BAUD = 0.8 MHz
    //MAX BAUD = 3.2 MHz
    spi_init(spi1, 1600000);

    //Configure data formats
    //Polarity = 1,  High when inactive
    //Phase = 0, Read the data on rising edge
    spi_set_format(spi1, 16, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);

    //Hardware Specific
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);

    //
    i2c_init(i2c1, 100000);

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi


    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
    // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    printf(" Hello, world!\n");
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
}

//Simple timer to toggle heartbeat led
bool HeartbeatCallback(repeating_timer_t *rt){
    bool state = gpio_get_out_level(HEARTBEAT);
    gpio_put(HEARTBEAT, !state);
    return true;
}