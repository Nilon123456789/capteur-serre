/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#define STRING(x) #x
#define TO_STRING(x) STRING(x)
#define LOCATION __FILE__ ":" TO_STRING(__LINE__)
#define RET_IF_ERR(expr, msg)                                   			\
    {                                                        								 	\
        int ret = (expr);                                    						 	 \
        if(ret) {                                            								   \
            LOG_ERR("Error %d: " msg " in " LOCATION, ret);  	  \
            return ret;                                       							   \
        }                                                       								\
    }

#define NAME_LEN 30
#define DATA_LEN 30

static struct service_data {
		uint8_t len;
		uint8_t data[DATA_LEN];
};

/**
 * @brief Send value to computer
 * 
 * @param name
 * @param addr
 * @param srv_data
 * @return static void
*/
static void send_value(char *name, char *addr, struct service_data *srv_data){
	printk("{%s,%s,", name, addr); // Print name and address
	for (int i = 0; i < srv_data->len-1; i++){ // Print service data
		printk("%02x-", srv_data->data[i]);
	}
	printk("%02x}\n", srv_data->data[srv_data->len - 1]); // Print last service data byte
}

/**
 * @brief Callback function for name
 * 
 * @param data
 * @param user_data
 * @return static bool
*/
static bool data_cb_name(struct bt_data *data, void *user_data)
{
	char *name = user_data;
	uint8_t len;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
		len = MIN(data->data_len, NAME_LEN - 1);
		(void)memcpy(name, data->data, len);
		name[len] = '\0';
		return false;
	default:
		return true;
	}
}

/**
 * @brief Callback function for service data
 * 
 * @param data
 * @param user_data
 * @return static bool
*/
static bool data_cb_service(struct bt_data *data, void *user_data){
	struct service_data *svc_data = user_data;

	switch (data->type) { // Get service data
	case BT_DATA_SVC_DATA16:  // 16-bit UUID
		svc_data->len = MIN(data->data_len, DATA_LEN - 1);
		(void)memcpy(svc_data->data, data->data, svc_data->len);
		svc_data->data[svc_data->len] = '\0';
		return false;
	default:
		return true;
	}
}

/**
 * @brief Callback function for received data
 * 
 * @param info
 * @param buf
 * @return static void
 * 
*/
static void scan_recv(const struct bt_le_scan_recv_info *info,
		      struct net_buf_simple *buf)
{
	char le_addr[BT_ADDR_LE_STR_LEN];
	char name[NAME_LEN];
	uint16_t buf_len = buf->len;
	struct net_buf_simple ad;
	struct service_data svc_data = {
		.len = 0,
		.data = {0},
	};

	(void)memset(name, 0, sizeof(name)); // Clear name

	(void)memcpy(&ad, buf, sizeof(ad)); // Copy buffer

	bt_data_parse(&ad, data_cb_name, name); // Get name

	if (name[0] == '\0') return; // If no name, ignore

	bt_data_parse(buf, data_cb_service, &svc_data); // Get service data

	if (svc_data.len < 1) return; // If no service data, ignore)
	
	bt_addr_to_str(&info->addr->a, le_addr, sizeof(le_addr)); // Get address

	LOG_INF("Received data from %s (%s)", le_addr, name);
	send_value(name, le_addr, &svc_data); // Send data to computer
}

static struct bt_le_scan_cb scan_callbacks = { 
	.recv = scan_recv,
};

void main(void)
{
	RET_IF_ERR(bt_enable(NULL), "Bluetooth init failed\n"); // Initialize Bluetooth
	
	bt_le_scan_cb_register(&scan_callbacks); // Register scan callback

	LOG_INF("Bluetooth initialized\n");

	struct bt_le_scan_param scan_param = { // Set scan parameters
		.type       = BT_LE_SCAN_TYPE_PASSIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = BT_GAP_SCAN_SLOW_INTERVAL_1,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};

	RET_IF_ERR(bt_le_scan_start(&scan_param, NULL), "Scanning failed to start\n"); // Start scanning
	LOG_INF("Scanning successfully started\n"); 
}
