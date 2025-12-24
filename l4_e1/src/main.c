#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SLEEP_TIME_MS 10 * 60 * 1000
#define SW0_NODE DT_NODELABEL(button0)
#define LED0_NODE DT_ALIAS(led0)

#define MAX_FACT 10

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
	long int factorial = 1;

	printk("Calculating the factorials of numbers 1 to %d:\n", MAX_FACT);
	for(int i = 1; i <= MAX_FACT; i++){
		factorial = factorial * i;
		printk("The factorial of %d is %ld\n", i, factorial);
	}
	printk("Done printing\n");
}

static struct gpio_callback button_cb_data;

int main(void)
{
	int ret;
	/* Only checking one since led.port and button.port point to the same device, &gpio0 */
	if (!device_is_ready(led.port)) {
		return -1;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return -1;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

	printk("nRF Connect SDK Fundamentals - Lesson 4 - Exercise 1\n");
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));

	gpio_add_callback(button.port, &button_cb_data);
	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}
