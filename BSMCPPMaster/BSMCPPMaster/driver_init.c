/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

#include <hpl_rtc_base.h>

/*! The buffer size for USART */
#define SERIAL_KNX_BUFFER_SIZE 16

/*! The buffer size for USART */
#define SERIAL_BSM_BUFFER_SIZE 16

struct usart_async_descriptor serial_knx;
struct usart_async_descriptor serial_bsm;
struct timer_descriptor       TIMER;

static uint8_t serial_knx_buffer[SERIAL_KNX_BUFFER_SIZE];
static uint8_t serial_bsm_buffer[SERIAL_BSM_BUFFER_SIZE];

struct flash_descriptor FLASH;

void FLASH_CLOCK_init(void)
{

	_pm_enable_bus_clock(PM_BUS_APBB, NVMCTRL);
}

void FLASH_init(void)
{
	FLASH_CLOCK_init();
	flash_init(&FLASH, NVMCTRL);
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void serial_knx_CLOCK_init()
{

	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void serial_knx_PORT_init()
{

	gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);

	gpio_set_pin_function(PA05, PINMUX_PA05D_SERCOM0_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void serial_knx_init(void)
{
	serial_knx_CLOCK_init();
	usart_async_init(&serial_knx, SERCOM0, serial_knx_buffer, SERIAL_KNX_BUFFER_SIZE, (void *)NULL);
	serial_knx_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void serial_bsm_CLOCK_init()
{

	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM3);
	_gclk_enable_channel(SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void serial_bsm_PORT_init()
{

	gpio_set_pin_function(PA22, PINMUX_PA22C_SERCOM3_PAD0);

	gpio_set_pin_function(PA23, PINMUX_PA23C_SERCOM3_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void serial_bsm_init(void)
{
	serial_bsm_CLOCK_init();
	usart_async_init(&serial_bsm, SERCOM3, serial_bsm_buffer, SERIAL_BSM_BUFFER_SIZE, (void *)NULL);
	serial_bsm_PORT_init();
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBA, RTC);
	_gclk_enable_channel(RTC_GCLK_ID, CONF_GCLK_RTC_SRC);
	timer_init(&TIMER, RTC, _rtc_get_timer());
}

void system_init(void)
{
	init_mcu();

	// GPIO on PB30

	gpio_set_pin_level(myLed,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(myLed, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(myLed, GPIO_PIN_FUNCTION_OFF);

	FLASH_init();

	serial_knx_init();
	serial_bsm_init();

	TIMER_init();
}
