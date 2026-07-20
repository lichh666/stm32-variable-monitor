#include "monitor_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MON_RX_BUF_SIZE 96

static mon_puts_fn g_puts = 0;
static mon_var_t *g_vars = 0;
static uint16_t g_var_count = 0;
static char g_rx_buf[MON_RX_BUF_SIZE];
static uint16_t g_rx_len = 0;

static void out(const char *s)
{
    if (g_puts) g_puts(s);
}

static mon_var_t *find_var(const char *name)
{
    uint16_t i;
    for (i = 0; i < g_var_count; i++) {
        if (strcmp(g_vars[i].name, name) == 0) return &g_vars[i];
    }
    return 0;
}

static int32_t read_var(mon_var_t *v)
{
    if (v->type == MON_VAR_U8) return *(uint8_t *)v->addr;
    if (v->type == MON_VAR_U16) return *(uint16_t *)v->addr;
    if (v->type == MON_VAR_U32) return (int32_t)(*(uint32_t *)v->addr);
    return *(int32_t *)v->addr;
}

static void write_var(mon_var_t *v, int32_t value)
{
    if (v->type == MON_VAR_U8) *(uint8_t *)v->addr = (uint8_t)value;
    else if (v->type == MON_VAR_U16) *(uint16_t *)v->addr = (uint16_t)value;
    else if (v->type == MON_VAR_U32) *(uint32_t *)v->addr = (uint32_t)value;
    else *(int32_t *)v->addr = value;
}

void Monitor_Init(mon_puts_fn put_fn, mon_var_t *vars, uint16_t count)
{
    g_puts = put_fn;
    g_vars = vars;
    g_var_count = count;
    g_rx_len = 0;
    out("BOOT STM32_RUNTIME_MONITOR 1.0\r\n");
    out("HELP: LIST | GET <name> | SET <name> <value> | PING\r\n");
}

void Monitor_OnChar(char ch)
{
    if (ch == '\r' || ch == '\n') {
        if (g_rx_len > 0) {
            g_rx_buf[g_rx_len] = 0;
            Monitor_ProcessLine(g_rx_buf);
            g_rx_len = 0;
        }
        return;
    }
    if (g_rx_len < MON_RX_BUF_SIZE - 1) {
        g_rx_buf[g_rx_len++] = ch;
    } else {
        g_rx_len = 0;
        out("ERR RX_OVERFLOW\r\n");
    }
}

void Monitor_ProcessLine(char *line)
{
    char *cmd = strtok(line, " ");
    char *arg1;
    char *arg2;
    char msg[128];
    mon_var_t *v;
    uint16_t i;

    if (!cmd) return;
    if (strcmp(cmd, "PING") == 0) {
        out("OK PONG\r\n");
        return;
    }
    if (strcmp(cmd, "HELP") == 0) {
        out("OK LIST | GET <name> | SET <name> <value> | PING\r\n");
        return;
    }
    if (strcmp(cmd, "LIST") == 0) {
        for (i = 0; i < g_var_count; i++) {
            snprintf(msg, sizeof(msg), "VAR %s %ld %s %c %s\r\n",
                     g_vars[i].name, (long)read_var(&g_vars[i]),
                     g_vars[i].unit ? g_vars[i].unit : "-",
                     g_vars[i].writable ? 'W' : 'R',
                     g_vars[i].desc ? g_vars[i].desc : "-");
            out(msg);
        }
        out("OK LIST_END\r\n");
        return;
    }
    if (strcmp(cmd, "GET") == 0) {
        arg1 = strtok(0, " ");
        if (!arg1) {
            out("ERR ARG\r\n");
            return;
        }
        v = find_var(arg1);
        if (!v) {
            out("ERR NO_VAR\r\n");
            return;
        }
        snprintf(msg, sizeof(msg), "DATA %s %ld %s\r\n",
                 v->name, (long)read_var(v), v->unit ? v->unit : "-");
        out(msg);
        return;
    }
    if (strcmp(cmd, "SET") == 0) {
        arg1 = strtok(0, " ");
        arg2 = strtok(0, " ");
        if (!arg1 || !arg2) {
            out("ERR ARG\r\n");
            return;
        }
        v = find_var(arg1);
        if (!v) {
            out("ERR NO_VAR\r\n");
            return;
        }
        if (!v->writable) {
            out("ERR READ_ONLY\r\n");
            return;
        }
        write_var(v, (int32_t)strtol(arg2, 0, 10));
        snprintf(msg, sizeof(msg), "OK SET %s %ld\r\n", v->name, (long)read_var(v));
        out(msg);
        return;
    }
    out("ERR UNKNOWN_CMD\r\n");
}

void Monitor_Periodic100ms(void)
{
    /* Reserved for periodic telemetry extension. */
}
