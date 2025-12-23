#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
// Using the button0 alias defined in the nrf52840 device tree file.
#define BTN0_NODE DT_NODELABEL(button0)

//Get the gpio structures for both the led and the button
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	//Make sure that both the led and the button are ready to be used
	if (!gpio_is_ready_dt(&led)) {
		printf("Error: LED device %s is not ready\n", led.port->name);
		return 0;
	}
	if (!gpio_is_ready_dt(&button)) {
		printf("Error: Button device %s is not ready\n", button.port->name);
		return 0;
	}

	//Configur the led as an output, and the button as an input
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	while (1) {
		//Poll at every instance to get the value of the button, then set the value of the led to that
		bool val = gpio_pin_get_dt(&button);
		ret = gpio_pin_set_dt(&led, val);
		if (ret < 0) {
			return 0;
		}

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
