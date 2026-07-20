#ifndef __BOARD_PORT_H
#define __BOARD_PORT_H

#include <stdint.h>

#ifndef ENABLE_LCD
#define ENABLE_LCD 0
#endif

void Board_Init(void);
void Board_LedSet(uint8_t index, uint8_t on);
void Board_BeepSet(uint8_t on);
uint8_t Board_KeyScan(void);
void Board_LcdShowStatus(int32_t temp_x10, int32_t limit_x10, uint32_t tick_s, uint8_t alarm);
void Board_UartPuts(const char *s);
void Board_DelayMs(uint32_t ms);

#endif
