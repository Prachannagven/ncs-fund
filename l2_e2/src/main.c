/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
//using the direct label for button 0. Should use sw0 as the alias for cross compatibility but whatever
#define BTN0_NODE DT_NODELABEL(button0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN0_NODE, gpios);

//Interrupt service routine is this function (what to do when the interrupt occurs)
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins){
	//Realistically, make this toggle to truly show how the isr routine works with this setup.
	gpio_pin_set_dt(&led, 1);
}

//Defining a callback variable
static struct gpio_callback button_cb_data;

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}
	if (!gpio_is_ready_dt(&btn)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	ret = gpio_pin_configure_dt(&btn, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button_cb_data, button_pressed, BIT(btn.pin));
	
	gpio_add_callback(btn.port, &button_cb_data);

	while (1) {
		//Technically speaking this goes back to the polling method of this, so i really should find something better. But you can just remove this entire if else loop and just leave the sleep
		if(gpio_pin_get_dt(&btn)){
			continue;
		}
		else {
			gpio_pin_set_dt(&led, 0);
		}
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
