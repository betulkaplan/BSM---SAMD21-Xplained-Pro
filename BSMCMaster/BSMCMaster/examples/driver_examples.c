/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static uint8_t src_data[128];
static uint8_t chk_data[128];
/**
 * Example of using FLASH to read and write Flash main array.
 */
void FLASH_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&FLASH, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&FLASH, 0x3200, chk_data, page_size);
}

/**
 * Example of using serial_knx to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

/*
static uint8_t example_serial_knx[12] = "Hello World!";

static void tx_cb_serial_knx(const struct usart_async_descriptor *const io_descr)
{
	/ * Transfer completed * /
}

void serial_knx_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&serial_knx, USART_ASYNC_TXC_CB, tx_cb_serial_knx);
	/ *usart_async_register_callback(&serial_knx, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&serial_knx, USART_ASYNC_ERROR_CB, err_cb);* /
	usart_async_get_io_descriptor(&serial_knx, &io);
	usart_async_enable(&serial_knx);

	io_write(io, example_serial_knx, 12);
}*/

/**
 * Example of using serial_bsm to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

/*
static uint8_t example_serial_bsm[12] = "Hello World!";

static void tx_cb_serial_bsm(const struct usart_async_descriptor *const io_descr)
{
	/ * Transfer completed * /
}

void serial_bsm_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&serial_bsm, USART_ASYNC_TXC_CB, tx_cb_serial_bsm);
	/ *usart_async_register_callback(&serial_bsm, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&serial_bsm, USART_ASYNC_ERROR_CB, err_cb);* /
	usart_async_get_io_descriptor(&serial_bsm, &io);
	usart_async_enable(&serial_bsm);

	io_write(io, example_serial_bsm, 12);
}

static struct timer_task TIMER_task1, TIMER_task2;
/ **
 * Example of using TIMER.
 * /
static void TIMER_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_example(void)
{
	TIMER_task1.interval = 100;
	TIMER_task1.cb       = TIMER_task1_cb;
	TIMER_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_task2.interval = 200;
	TIMER_task2.cb       = TIMER_task2_cb;
	TIMER_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER, &TIMER_task1);
	timer_add_task(&TIMER, &TIMER_task2);
	timer_start(&TIMER);
}
*/
