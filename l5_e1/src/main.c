#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

//UART required definitions
#define TIMEOUT_uS		100
#define BUF_SIZE_BYTES	10

// The devicetree node identifier for all the LEDs
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

//Commonly usd short forms
#define DAT_BUF evt->data.rx.buf
#define DAT_LEN evt->data.rx.len

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec LED0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec LED3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
static uint8_t rx_buf[BUF_SIZE_BYTES] = {0};
static uint8_t tx_buf[] =   {"nRF Connect SDK Fundamentals Course\r\n"
                             "Press 1-4 on your keyboard to toggle LEDS 1-4 on your development kit\r\n"};

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data){
	switch (evt->type){
		case UART_RX_RDY:
			if(DAT_LEN == 1){
				if(evt->data.rx.buf[evt->data.rx.offset] == '1'){
					gpio_pin_toggle_dt(&LED0);
				}
				if(evt->data.rx.buf[evt->data.rx.offset] == '2'){
					gpio_pin_toggle_dt(&LED1);
				}
				if(evt->data.rx.buf[evt->data.rx.offset] == '3'){
					gpio_pin_toggle_dt(&LED2);
				}
				if(evt->data.rx.buf[evt->data.rx.offset] == '4'){
					gpio_pin_toggle_dt(&LED3);
				}
			}
			else{
				printk("Invalid entry. Only type a character from 1, 2, 3, 4\r\n");
			}
			break;
		case UART_RX_DISABLED:
			uart_rx_enable(dev, rx_buf, sizeof rx_buf, TIMEOUT_uS);
			break;
		default:
			break;
	}
}

int main(void)
{
	int ret;

	//Make sure the gpios and the uart are ready
    if (!device_is_ready(LED0.port)) {
        printk("GPIO device is not ready\n");
        return 1;
    }
	if (!device_is_ready(uart)) {
		return 0;
	}

	//Configure all the LEDs to be outputs
	ret = gpio_pin_configure_dt(&LED0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&LED1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&LED2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&LED3, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = uart_callback_set(uart, uart_cb, NULL);
	if(ret){
		return 1;
	}

	ret = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
	if (ret) {
		return 1;
	}

	ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, TIMEOUT_uS);
	if (ret) {
		return 1;
	}
	while(1){
		k_msleep(SLEEP_TIME_MS);
	}
}
