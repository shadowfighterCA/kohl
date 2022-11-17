#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"
#include "esp_ble_mesh_lighting_model_api.h"

#include "../board.h"
#include "ble_mesh_example_init.h"

#define LEVEL_TAG "LEVEL_TAG"

//level server config

ESP_BLE_MESH_MODEL_PUB_DEFINE(level_pub, 2 + 3, ROLE_NODE);
static esp_ble_mesh_gen_level_srv_t level_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
};

// level server model

static esp_ble_mesh_model_t level_model[] = {

    ESP_BLE_MESH_MODEL_GEN_LEVEL_SRV(&level_pub,&level_server),
};



static void example_change_level(esp_ble_mesh_model_t *model,
                                     esp_ble_mesh_msg_ctx_t *ctx, int16_t level)
{
    uint16_t primary_addr = esp_ble_mesh_get_primary_element_address();
    uint8_t elem_count = esp_ble_mesh_get_element_count();
   // struct _led_state *led = NULL;
    uint8_t i;

    if (ESP_BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        for (i = 0; i < elem_count; i++) {
            if (ctx->recv_dst == (primary_addr + i)) {
                //led = &led_state[i];
               // board_led_operation(led->pin, onoff);

               ESP_LOGI(LEVEL_TAG,"level = %d",level);
            }
        }
    } else if (ESP_BLE_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        if (esp_ble_mesh_is_model_subscribed_to_group(model, ctx->recv_dst)) {
           // led = &led_state[model->element->element_addr - primary_addr];
            //board_led_operation(led->pin, onoff);
            ESP_LOGI(LEVEL_TAG,"level = %d",level);
        }
    } else if (ctx->recv_dst == 0xFFFF) {
       // led = &led_state[model->element->element_addr - primary_addr];
        //board_led_operation(led->pin, onoff);
        ESP_LOGI(LEVEL_TAG,"level = %d",level);
    }
}

// callback function

static void example_handle_gen_level_msg(esp_ble_mesh_model_t *model,
                                         esp_ble_mesh_msg_ctx_t *ctx,
                                         esp_ble_mesh_server_recv_gen_level_set_t *set)
{
    esp_ble_mesh_gen_level_srv_t *srv = model->user_data;

    switch (ctx->recv_op) {
    case ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_GET:
        esp_ble_mesh_server_model_send_msg(model, ctx,
            ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS, sizeof(srv->state.level), &srv->state.level);
        break;
    case ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
    case ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:
        if (set->op_en == false) {
            srv->state.level = set->level;
        } else {
            /* TODO: Delay and state transition */
            srv->state.level = set->level;
        }
        if (ctx->recv_op == ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET) {
            esp_ble_mesh_server_model_send_msg(model, ctx,
                ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS, sizeof(srv->state.level), &srv->state.level);
        }
        esp_ble_mesh_model_publish(model, ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS,
            sizeof(srv->state.level), &srv->state.level, ROLE_NODE);
       // example_change_led_state(model, ctx, srv->state.onoff);
       example_change_level(model,ctx,srv->state.level);
        break;
    default:
        break;
    }
}