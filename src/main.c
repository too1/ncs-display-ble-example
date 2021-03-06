/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include "gui.h"
#include "app_ble.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

static void on_app_ble_event(app_ble_event_data_t * evt_data)
{
	switch(evt_data->type) {
		case APP_BLE_CONNECTED:
			printk("Connected\n");
			gui_set_bt_state(GUI_BT_STATE_CONNECTED);
			break;

		case APP_BLE_DISCONNECTED:
			printk("Disconnected\n");
			gui_set_bt_state(GUI_BT_STATE_ADVERTISING);

			app_ble_start_advertising();
			break;

		case APP_BLE_ON_LED_WRITE:
			printk("Led %s\n", evt_data->led_state ? "On" : "Off");
			gui_set_bt_led_state(evt_data->led_state);
			break;
	}
}

void on_gui_event(gui_event_t *event)
{
	switch(event->evt_type) {
		case GUI_EVT_BUTTON_PRESSED:
			app_ble_send_button_state(event->button_checked);
			break;
	}
}

void main(void)
{
	gui_config_t gui_config = {.event_callback = on_gui_event};
	gui_init(&gui_config);

	app_ble_config_t app_ble_config = {.event_callback = on_app_ble_event};
	app_ble_init(&app_ble_config);

	app_ble_start_advertising();

	printk("Advertising started\n");

	gui_set_bt_state(GUI_BT_STATE_ADVERTISING);

	while (1) {
		k_sleep(K_MSEC(1000));
	}
}
