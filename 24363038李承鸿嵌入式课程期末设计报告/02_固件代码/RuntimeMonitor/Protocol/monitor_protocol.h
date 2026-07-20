#ifndef __MONITOR_PROTOCOL_H
#define __MONITOR_PROTOCOL_H

#include <stdint.h>

typedef enum {
    MON_VAR_U8 = 0,
    MON_VAR_U16,
    MON_VAR_U32,
    MON_VAR_I32,
} mon_var_type_t;

typedef struct {
    const char *name;
    mon_var_type_t type;
    void *addr;
    uint8_t writable;
    const char *unit;
    const char *desc;
} mon_var_t;

typedef void (*mon_puts_fn)(const char *s);

void Monitor_Init(mon_puts_fn put_fn, mon_var_t *vars, uint16_t count);
void Monitor_OnChar(char ch);
void Monitor_ProcessLine(char *line);
void Monitor_Periodic100ms(void);

#endif
