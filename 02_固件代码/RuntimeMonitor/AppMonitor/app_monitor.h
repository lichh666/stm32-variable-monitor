#ifndef __APP_MONITOR_H
#define __APP_MONITOR_H

#include <stdint.h>

void AppMonitor_Init(void);
void AppMonitor_Tick100ms(void);
void AppMonitor_Loop(void);
void AppMonitor_OnUartChar(char ch);

#endif
