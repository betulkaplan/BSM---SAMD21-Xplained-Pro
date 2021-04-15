/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_flash.h>

#include <hal_usart_async.h>
#include <hal_usart_async.h>
#include <hal_timer.h>

extern struct flash_descriptor       FLASH;
extern struct usart_async_descriptor serial_knx;
extern struct usart_async_descriptor serial_bsm;
extern struct timer_descriptor       TIMER;

void FLASH_init(void);
void FLASH_CLOCK_init(void);

void serial_knx_PORT_init(void);
void serial_knx_CLOCK_init(void);
void serial_knx_init(void);

void serial_bsm_PORT_init(void);
void serial_bsm_CLOCK_init(void);
void serial_bsm_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
