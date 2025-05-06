 #include "cal_funcs.h"

 //Per Documentation first 3 cycles are acquiring voltage
 // Next 13 cycles conversion is complete and data is clocked out
 // MSB First staring on 5th clock.
 // Store previous read 
 // 
 // All data is transfered MSB first
 uint16_t SPI_ReadADC(const uint8_t channel){
     static uint16_t prev_addr = 0u;
 
     //ensure the address is only 2 bits.
     uint16_t address = channel && 0x3;
     uint16_t data = 0;
 
     address = address << (16 - 3);
     //Durring first 8 clocks we write the address of input to read
     //If read is same we can use the first read value, otherwise we delay one cycle
     if (address != prev_addr){
         spi_write16_read16_blocking(spi0, &address, &data, 1);
     }
     spi_write16_read16_blocking(spi0, &address, &data, 1);
     prev_addr = address;
 
     //Ensure only 12 bits that are measured are sent
     data = data && 0x0FFF;
 
     return data;
 }
 
 
uint16_t GetMotorCurrent(const uint8_t pole){
    //Map each pole to a specific spi_read

    //CS can be determined by & 0x4, 0-3 are cs 1, 4-5 are cs 2
    bool chip_select = (pole & 0x4) != 0 ;
    float output = 0.0f;
    gpio_put(I_SENSE_1_CS, !chip_select);
    gpio_put(I_SENSE_2_CS, chip_select);
    
    int32_t counts = 0;

    for (auto i = 0; i < 5; i++){
        //Average 5 readings
        //Valid channels are 0,1,2,3
        counts = static_cast<int32_t>(SPI_ReadADC(pole & (!0x4))) - 2048;

        //convert counts into a mA reading.
        output += static_cast<float>(counts) * 7.0f;
    }
    
    //Reset CS to be high
    gpio_put(I_SENSE_1_CS, 1);
    gpio_put(I_SENSE_2_CS, 1);

    output /= static_cast<float>(5);
    
    return static_cast<uint16_t>(output);
}
 
 uint8_t CheckLimSw(const uint8_t sw_time){
     bool z_good = false;
     bool idx_good = false;
     bool pull_good = false;
     uint8_t output = 0;
     //Invert Switch Actions
     //Positive logic, 1 is inverted 0 is normal
     gpio_put(Z_INV, true);
     gpio_put(IDX_INV, true);
     gpio_put(PULL_INV, true);
    
     //wait for 50ms to ensure switch has time to change modes
     sleep_ms(static_cast<uint32_t>(sw_time));
 
     //Expect all to be High, indicating sw not blocked
     z_good = gpio_get(Z_MONITOR);
     idx_good = gpio_get(IDX_MONITOR);
     pull_good = gpio_get(PULL_MONITOR);
 
     //Revert all limit switches to normal operating conditions
     gpio_put(Z_INV, false);
     gpio_put(IDX_INV, false);
     gpio_put(PULL_INV, false);
     
     //0b0000_0zip;
     output = (z_good << 2) + (idx_good << 1) + (pull_good);
 
     return output;
 }
