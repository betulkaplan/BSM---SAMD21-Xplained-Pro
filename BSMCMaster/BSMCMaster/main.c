#include <atmel_start.h>

static void tx_cb_serial_knx(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

uint8_t knx_testArray[9] = {0x00};
uint8_t knx_ptr = 0;
static void rx_cb_serial_knx(const struct usart_async_descriptor *const io_descr)
{
	uint8_t ch, count;
	count = io_read(&serial_knx.io, &ch, 1);
	knx_testArray[knx_ptr] = ch;
	knx_ptr++;
	if (knx_ptr == 9)
	{
		knx_ptr = 0;
	}
}

void serial_knx_Init(void)
{
	/* TX-> PA04 / RX-> PA05 */
	struct io_descriptor *io;

	usart_async_register_callback(&serial_knx, USART_ASYNC_TXC_CB, tx_cb_serial_knx);
	usart_async_register_callback(&serial_knx, USART_ASYNC_RXC_CB, rx_cb_serial_knx);
	/*usart_async_register_callback(&serial_knx, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&serial_knx, &io);
	usart_async_enable(&serial_knx);

}


static void tx_cb_serial_bsm(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

uint8_t testArray[9] = {0x00};
uint8_t ptr = 0;
static void rx_cb_serial_bsm(const struct usart_async_descriptor *const io_descr)
{
	uint8_t ch, count;
	count = io_read(&serial_bsm.io, &ch, 1);
	testArray[ptr] = ch;
	ptr++;
	if (ptr == 9)
	{
		ptr = 0;
	}
}

void serial_bsm_Init(void)
{
	/* TX-> PA22 / RX-> PA23 */
	struct io_descriptor *io;

	usart_async_register_callback(&serial_bsm, USART_ASYNC_TXC_CB, tx_cb_serial_bsm);
	usart_async_register_callback(&serial_bsm, USART_ASYNC_RXC_CB, rx_cb_serial_bsm);
	/*usart_async_register_callback(&serial_bsm, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&serial_bsm, &io);
	usart_async_enable(&serial_bsm);
}

static struct timer_task TIMER_task1, TIMER_task2;
/**
 * Example of using TIMER.
 */
static void TIMER_task1_cb(const struct timer_task *const timer_task)
{
	//gpio_toggle_pin_level(myLed);
}

static void TIMER_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_Init(void)
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

#include "utils.h"

static uint8_t src_data[128];
static uint8_t chk_data[128];


void FLASH_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH);

	for (i = 0; i < 128; i++) {
		src_data[i] = 0xaa;
	}
	//src_data[0]=0x17;

	/* Write data to flash */
	flash_write(&FLASH, 0x3200, src_data, 128);
	
	uint8_t dum[4] = {0x17, 0x52, 0x98, 0x13};
	flash_write(&FLASH, 0x3201, dum, 4);
	dum[3] = 0x33;
	flash_write(&FLASH, 0x3201, dum, 4);
	
	
	/* Read data from flash */
	flash_read(&FLASH, 0x3200, chk_data, 128);
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	serial_knx_Init();
	serial_bsm_Init();
	TIMER_Init();
	FLASH_example();
	
	
	/* Replace with your application code */
	while (1) {
	}
}
