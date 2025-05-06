#ifndef TASKS_H
#define TASKS_H

void process_commands();
void read_loadcells();
void update_light();
void read_motor_current();

uint16_t GetMotorCurrent(const uint8_t pole);
uint16_t SPI_ReadADC(const uint8_t channel);

#endif