#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;
	if(!gpio_is_ready_dt(&led0)) {
		printf("Error: LED device %s is not ready\n", led0.port->name);
		return 0;
	}
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printf("Error %d: failed to configure LED pin %d\n", ret, led0.pin);
		return 0;
	}

	while (1) {
		printk("Hello World!\n");
		gpio_pin_toggle_dt(&led0);
		k_msleep(1000);
	}
}