#ifndef LIB_H
#define LIB_H

// Part of the code downloaded from https://github.com/Spivoxity/microbian 
// lib.h
// Copyright (c) 2018 J. M. Spivey

#include <stdarg.h>

/* do_print -- the device-independent guts of printf */
void do_print(void (*putch)(char), const char *fmt, va_list va);

/* printf -- print using putchar */
void printf(const char *fmt, ...);

/* sprintf -- print to string buffer.  Note danger of overflow! */
int sprintf(char *buf, const char *fmt, ...);

/* atoi -- convert decimal string to int */
int atoi(const char *p);

/* xtou -- convert hex string to unsigned */
unsigned xtou(char *p);

#endif  /* LIB_H */
