/**
 * ble.c
 * 
 * BLE driver
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "ble.h"
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>

LOG_MODULE_REGISTER(BLE_DRIVER, CONFIG_BLE_DRIVER_LOG_LEVEL);

static int counter;

static bool isInisialized = false;

static uint8_t service_data[22] = {0};

static bt_addr_t addr;

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_SVC_DATA16, service_data, sizeof(service_data)),
};

static struct bt_le_ext_adv *adv;

struct bt_le_adv_param adv_param = {
		.secondary_max_skip = 0U,
		.options = (BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_USE_IDENTITY),
		.interval_min = ((uint16_t)(CONFIG_BLE_MIN_ADV_INTERVAL_MS) / 0.625f),
		.interval_max = ((uint16_t)(CONFIG_BLE_MAX_ADV_INTERVAL_MS) / 0.625f),
		.peer = NULL,
};

/**
 * @brief Initialize the BLE driver
 * 
 * @return int 0 if no error, error code otherwise
*/
int ble_init(void) {

    if(isInisialized) {
        LOG_WRN("BLE already initialized");
        return 0;
    }

    LOG_INF("Setting custom mac addr to: %s", CONFIG_BLE_USER_DEFINED_MAC_ADDR);
    RET_IF_ERR(bt_addr_le_from_str(&CONFIG_BLE_USER_DEFINED_MAC_ADDR, "random", &addr), "Unable to converte mac addr");
    RET_IF_ERR(bt_id_create(&addr, NULL), "Unable to set mac addr");

    RET_IF_ERR(bt_enable(NULL), "Bluetooth init failed");

    RET_IF_ERR(bt_le_ext_adv_create(&adv_param, NULL, &adv), "Advertising failed to create");

    /* Setting service UUID */
    service_data[0] = SERVICE_UUID_1;
    service_data[1] = SERVICE_UUID_2;

    isInisialized = true;
    LOG_INF("Bluetooth initialized");

    return 0;
}

/**
 * @brief quickly encode a pair of float values into the service data
 * 
 * @param pos position in the service data array
 * @param id id of the value
 * @param val value to encode
 * 
 * @return int 0 if no error, error code otherwise
*/
static int ble_encode_pair(uint8_t pos, uint8_t id, float *val) {
    uint8_t whole, decimal;

    RET_IF_ERR(floatSeparator(val, &whole, &decimal), "Unable to separate float");

    service_data[pos] = id;
    service_data[pos + 1] = whole;
    service_data[pos + 2] = decimal;

    return 0;
}

/**
 * @brief Encode the data into the service data
 * 
 * @param temp temperature
 * @param hum humidity
 * @param lumino luminosity
 * @param gnd_tmp ground temperature
 * @param gnd_hum ground humidity
 * @param battery battery level
 * 
 * @return int 0 if no error, error code otherwise
*/
int ble_encode_adv_data(float *temp, float *hum, float *lumino, float *gnd_hum, float *gnd_tmp, float *battery) {

    /* Setting counter */
    service_data[2] = 0;
    service_data[3] = counter;

    /* Setting data */
    RET_IF_ERR(ble_encode_pair(4, TEMP_ID, temp), "Unable to encode temperature");
    RET_IF_ERR(ble_encode_pair(7, HUM_ID, hum), "Unable to encode humidity");
    RET_IF_ERR(ble_encode_pair(10, LUM_ID, lumino), "Unable to encode luminosity");
    RET_IF_ERR(ble_encode_pair(13, GND_TEMP_ID, gnd_tmp), "Unable to encode ground temperature");
    RET_IF_ERR(ble_encode_pair(16, GND_HUM_ID, gnd_hum), "Unable to encode ground humidity");
    RET_IF_ERR(ble_encode_pair(19, BAT_ID, battery), "Unable to encode battery");

    return 0;
}

/**
 * @brief Start advertising for a given duration (from config)
 * 
 * @return int 0 if no error, error code otherwise
*/
int ble_adv() {
    if (!isInisialized) {
        LOG_ERR("BLE not initialized");
        return -1;
    }
    LOG_INF("Starting advertising #%d", counter);

    RET_IF_ERR(bt_le_ext_adv_set_data(adv, ad, ARRAY_SIZE(ad), NULL, 0), "Advertising failed to set data");

    RET_IF_ERR(bt_le_ext_adv_start(adv, BT_LE_EXT_ADV_START_DEFAULT), "Advertising failed to start");
    
    LOG_INF("Advertising started for %d seconds", CONFIG_BLE_ADV_DURATION_SEC);

    /* Wait for advertising to end */
    k_sleep(K_SECONDS(CONFIG_BLE_ADV_DURATION_SEC));

    /* Stop advertising */
    RET_IF_ERR(bt_le_ext_adv_stop(adv), "Advertising failed to stop");
    LOG_INF("Advertising stopped");

    /* Increment counter */
    counter++;

    return 0;
}