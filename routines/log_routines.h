#ifndef LOG_ROUTINES_H
#define LOG_ROUTINES_H

#define DEBUG_ON

// Debug print macro
#ifdef DEBUG_ON
#define DEBUG_PRINT(fmt, ...) KPrintF(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

#endif // LOG_ROUTINES_H