/* main.c - Application main entry point */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"
#include "esp_ble_mesh_lighting_model_api.h"

#include "board.h"
#include "ble_mesh_example_init.h"
#include "servers/root_server_config.h"
#include "servers/onoff_server_config.h"
#include "servers/level_server_config.h"
#include "servers/hsl_server_config.h"
#include "led_strip/led_strip_config.h"
//#include "Tasks/task1.h"
//#include "led_strip/led_task.h"
//#include "driver/rmt.h"
//#include "led_strip.h"

#define TAG "MAIN_TAG"

#define CID_ESP 0x02E5



static uint8_t dev_uuid[16] = { 0xdd, 0xdd };
int is_provisioned = 0;
int is_identified = 0;

void function1(void* pvParameters){

int i;

    while(1){


        if(!is_provisioned){
            for(i=0;i<55;i++){
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 255, 0, 0));
             ESP_ERROR_CHECK(strip->refresh(strip, 100));
            vTaskDelay(pdMS_TO_TICKS(100));
            }


        }
        else{

            if(!is_received){
            for(i=0;i<55;i++){
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0, 255, 0));
             ESP_ERROR_CHECK(strip->refresh(strip, 100));
            vTaskDelay(pdMS_TO_TICKS(100));
            }

            for(i=0;i<55;i++){
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0, 0, 255));
             ESP_ERROR_CHECK(strip->refresh(strip, 100));
            vTaskDelay(pdMS_TO_TICKS(100));
            }
            
            
            
            }




            else{
                for(i=0;i<55;i++){
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, red, green, blue));
             ESP_ERROR_CHECK(strip->refresh(strip, 100));
            vTaskDelay(pdMS_TO_TICKS(10));
                
                }



            }
        }


    }



}

//static const char *TAG = "example";


static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(0, extend_model_0, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(0, level_model, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(0, HSL_model, ESP_BLE_MESH_MODEL_NONE),  // HSL model
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .elements = elements,
    .element_count = ARRAY_SIZE(elements),
};

/* Disable OOB security for SILabs Android app */
static esp_ble_mesh_prov_t provision = {
    .uuid = dev_uuid,
#if 0
    .output_size = 4,
    .output_actions = ESP_BLE_MESH_DISPLAY_NUMBER,
    .input_actions = ESP_BLE_MESH_PUSH,
    .input_size = 4,
#else
    .output_size = 0,
    .output_actions = 0,
#endif
};

static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    ESP_LOGI(TAG, "net_idx: 0x%04x, addr: 0x%04x", net_idx, addr);
    ESP_LOGI(TAG, "flags: 0x%02x, iv_index: 0x%08x", flags, iv_index);
    board_led_operation(LED_G, LED_OFF);
}









static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code %d", param->prov_register_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT, err_code %d", param->node_prov_enable_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT, bearer %s",
            param->node_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
            is_identified = 1;

        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT, bearer %s",
            param->node_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT");
        prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr,
            param->node_prov_complete.flags, param->node_prov_complete.iv_index);
            is_provisioned = 1;
            is_identified = 0;
        break;
    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_RESET_EVT");
        break;
    case ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT, err_code %d", param->node_set_unprov_dev_name_comp.err_code);
        break;
    default:
        break;
    }
}

static void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
                                              esp_ble_mesh_cfg_server_cb_param_t *param)
{
    if (event == ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT) {
        switch (param->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD");
            ESP_LOGI(TAG, "net_idx 0x%04x, app_idx 0x%04x",
                param->value.state_change.appkey_add.net_idx,
                param->value.state_change.appkey_add.app_idx);
            ESP_LOG_BUFFER_HEX("AppKey", param->value.state_change.appkey_add.app_key, 16);
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND");
            ESP_LOGI(TAG, "elem_addr 0x%04x, app_idx 0x%04x, cid 0x%04x, mod_id 0x%04x",
                param->value.state_change.mod_app_bind.element_addr,
                param->value.state_change.mod_app_bind.app_idx,
                param->value.state_change.mod_app_bind.company_id,
                param->value.state_change.mod_app_bind.model_id);
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD");
            ESP_LOGI(TAG, "elem_addr 0x%04x, sub_addr 0x%04x, cid 0x%04x, mod_id 0x%04x",
                param->value.state_change.mod_sub_add.element_addr,
                param->value.state_change.mod_sub_add.sub_addr,
                param->value.state_change.mod_sub_add.company_id,
                param->value.state_change.mod_sub_add.model_id);
            break;
        default:
            break;
        }
    }
}



static void example_ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                               esp_ble_mesh_generic_server_cb_param_t *param)
{
    esp_ble_mesh_gen_onoff_srv_t *srv;
    esp_ble_mesh_gen_level_srv_t *srv1;
    ESP_LOGI(ROOT_TAG, "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {

  

    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        ESP_LOGI(ROOT_TAG, "ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
            param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            ESP_LOGI(ROOT_TAG, "onoff 0x%02x", param->value.state_change.onoff_set.onoff);
            example_change_led_state(param->model, &param->ctx, param->value.state_change.onoff_set.onoff);
        }

                if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET ||
            param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK) {
            ESP_LOGI(ROOT_TAG, "level 0x%02x", param->value.state_change.level_set.level);
           example_change_level(param->model, &param->ctx, param->value.state_change.level_set.level);
        }







        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
        ESP_LOGI(ROOT_TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
            srv = param->model->user_data;
            ESP_LOGI(ROOT_TAG, "onoff 0x%02x", srv->state.onoff);
            example_handle_gen_onoff_msg(param->model, &param->ctx, NULL);
        }

        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_GET) {
         srv1 = param->model->user_data;
            ESP_LOGI(ROOT_TAG, "level 0x%02x", srv1->state.level);
            example_handle_gen_level_msg(param->model, &param->ctx, NULL);
        }





        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
        ESP_LOGI(ROOT_TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
            param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            ESP_LOGI(ROOT_TAG, "onoff 0x%02x, tid 0x%02x", param->value.set.onoff.onoff, param->value.set.onoff.tid);
            if (param->value.set.onoff.op_en) {
                ESP_LOGI(ROOT_TAG, "trans_time 0x%02x, delay 0x%02x",
                    param->value.set.onoff.trans_time, param->value.set.onoff.delay);
            }
            example_handle_gen_onoff_msg(param->model, &param->ctx, &param->value.set.onoff);
        }

        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET ||
            param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK) {
            ESP_LOGI(ROOT_TAG, "level 0x%02x, tid 0x%02x", param->value.set.level.level, param->value.set.level.tid);
            if (param->value.set.level.op_en) {
                ESP_LOGI(ROOT_TAG, "trans_time 0x%02x, delay 0x%02x",
                    param->value.set.level.trans_time, param->value.set.level.delay);
            }
            example_handle_gen_level_msg(param->model, &param->ctx, &param->value.set.level);
        }


        
                
        break;

    default:
        ESP_LOGE(ROOT_TAG, "Unknown Generic Server event 0x%02x", event);
        break;

    }
}






static esp_err_t ble_mesh_init(void)
{
    esp_err_t err = ESP_OK;

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_config_server_callback(example_ble_mesh_config_server_cb);
    esp_ble_mesh_register_lighting_server_callback(example_ble_mesh_lighting_server_cb);
    esp_ble_mesh_register_generic_server_callback(example_ble_mesh_generic_server_cb);
   

    err = esp_ble_mesh_init(&provision, &composition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize mesh stack (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable mesh node (err %d)", err);
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Node initialized");
   // strip->set_pixel(strip, 3, 13, 92, 219);

    board_led_operation(LED_G, LED_ON);

    return err;
}

;


void app_main(void)
{
    // Led strip initializing
    int i;


             
    rmt_led_init();
   

     
   

    // install ws2812 driver
 


    if (!strip) {
        ESP_LOGI(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    // Show simple rainbow chasing pattern
    ESP_LOGI(TAG, "LED Rainbow Chase Start");


    // LED initializing over
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    board_init();

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }


        xTaskCreate(&function1,"Task1",1024,NULL,1,NULL);
}
