#include <atmel_start.h>
#include "..\knx_lib\KNX.h"

class atmel_knx: public knx_base
{
	 	public:
	// 	void device_init();
	// 	void timer_init();
	// 	void com_init();
	// 	void TXchar(char);
	// 	void wait(uint16_t);
		void writeToMemory(uint16_t startAdd, uint8_t *buffer, uint16_t size);
		void readFromMemory(uint16_t startAdd, void *buffer, uint16_t size);
	// 	int GroupAddIsExistent(uint16_t group_add);
	// 	uint16_t getFirstGroupAddressofObj(uint8_t object_index);
	// 	void writeObjects(uint16_t group_address, uint8_t* value,knx_ObjectBase*);
	// 	uint16_t getIndAddress();
	// 	void setIndAddress(uint16_t);
	//
	// 	knx_ObjectBase* getObject(int index);
};

atmel_knx knx;

void atmel_knx::writeToMemory(uint16_t startAdd, uint8_t *buffer, uint16_t size)
{

	/*uint16_t xstartAdd = convert_eepromAdd(startAdd);
	if (startAdd == 0) return; */
	flash_write(&FLASH, startAdd, buffer, size);
	//eeprom_write_block(buffer, (void *)xstartAdd, size);
	
}
void atmel_knx::readFromMemory(uint16_t startAdd, void *buffer, uint16_t size)
{
	/*uint16_t xstartAdd = convert_eepromAdd(startAdd);
	if (startAdd == 0) return;
	eeprom_read_block(buffer, (void *)xstartAdd, size);*/
}

void USART_onRX(char c)
{
	knx.on_RXchar(c);
}

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
	USART_onRX(ch);
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




int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	serial_knx_Init();
	serial_bsm_Init();
	TIMER_Init();
	
	uint8_t dumBuff[7];
	for (int i=0; i<7; i++)
	{
		dumBuff[i]=0x9;
	}
	knx.writeToMemory(0x5600, dumBuff, 7);
	
	uint8_t dumBuff2[1] = {0x22};
	
	knx.writeToMemory(0x5603, dumBuff2, 1);
	
	
	
	/***************************************/
	static uint8_t src_data[128];
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	//flash_write(&FLASH, 0x5600, src_data, page_size);

	/* Replace with your application code */
	while (1) {
	}
}
