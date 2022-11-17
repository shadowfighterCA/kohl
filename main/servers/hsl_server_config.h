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
#include "led_strip/led_strip_config.h"
//#include "led_strip/led_task.h"
#include "freertos/queue.h"

#define HSL_TAG "HSL_SERVER_TAG"

static esp_ble_mesh_light_hsl_state_t hsl_state;

int is_received = 0;
int r,g,b;
int h,s,l;



void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}


void rmt_led_init(){


       rmt_config112.channel = RMT_TX_CHANNEL;                     
        rmt_config112.gpio_num = GPIO_NUM_9;                            
        rmt_config112.clk_div = 80;                               
        rmt_config112.mem_block_num = 1;                          
        rmt_config112.flags = 0;                                                        
            rmt_config112.tx_config.carrier_freq_hz = 38000;                
            rmt_config112.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH; 
            rmt_config112.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;        
            rmt_config112.tx_config.carrier_duty_percent = 33;              
            rmt_config112.tx_config.carrier_en = false;                    
            rmt_config112.tx_config.loop_en = false;                        
            rmt_config112.tx_config.idle_output_en = true;    

               strip_config.max_leds = 55;
               strip_config.dev = (led_strip_dev_t)rmt_config112.channel;
        
     rmt_config112.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&rmt_config112));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_config112.channel, 0, 0));
    
    strip = led_strip_new_rmt_ws2812(&strip_config);


}





// HSL server ini
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_srv_t hsl_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,


};
//done
// HSL setup server
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_setup_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_setup_srv_t hsl_setup_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};
//done
 
 //HSL models
static esp_ble_mesh_model_t HSL_model[] = {

   ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(&hsl_setup_pub,&hsl_setup_server),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_pub,&hsl_server),
};


//actual function



//HSL function
static void example_change_hsl(esp_ble_mesh_model_t *model,
                                     esp_ble_mesh_msg_ctx_t *ctx, uint32_t hue,uint32_t saturation, uint32_t lightness )
{

    is_received =1;
    uint16_t primary_addr = esp_ble_mesh_get_primary_element_address();
    uint8_t elem_count = esp_ble_mesh_get_element_count();
   // struct _led_state *led = NULL;
    uint8_t i;

    if (ESP_BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        for (i = 0; i < elem_count; i++) {
            if (ctx->recv_dst == (primary_addr + i)) {
                //led = &led_state[i];
               // board_led_operation(led->pin, onoff);

               ESP_LOGI(HSL_TAG,"Hue = %d, Saturation = %d, Lightness = %d ",hue,saturation,lightness);
              


                    led_strip_hsv2rgb(hue,saturation,lightness,&red,&green,&blue);

                 ESP_LOGI(HSL_TAG," After Red = %d, Green = %d, Blue = %d ",red,green,blue);
          
            }
        }
    } else if (ESP_BLE_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        if (esp_ble_mesh_is_model_subscribed_to_group(model, ctx->recv_dst)) {
           // led = &led_state[model->element->element_addr - primary_addr];
            //board_led_operation(led->pin, onoff);
            //ESP_LOGI(TAG,"level = %d",level);
            ESP_LOGI(HSL_TAG,"Hue = %d, Saturation = %d, Lightness = %d ",hue,saturation,lightness);
           

                 led_strip_hsv2rgb(hue,saturation,lightness,&red,&green,&blue);

                  ESP_LOGI(HSL_TAG," After Red = %d, Green = %d, Blue = %d ",red,green,blue);

             
        }
    } else if (ctx->recv_dst == 0xFFFF) {
       // led = &led_state[model->element->element_addr - primary_addr];
        //board_led_operation(led->pin, onoff);
        //ESP_LOGI(TAG,"level = %d",level);
        ESP_LOGI(HSL_TAG,"Hue = %d, Saturation = %d, Lightness = %d ",hue,saturation,lightness);
        ESP_LOGI(HSL_TAG," Before Red = %d, Green = %d, Blue = %d ",red,green,blue);


                    led_strip_hsv2rgb(hue,saturation,lightness,&red,&green,&blue);

                    ESP_LOGI(HSL_TAG," After Red = %d, Green = %d, Blue = %d ",red,green,blue);

    }
}



static void example_handle_HSL_msg(esp_ble_mesh_model_t *model,
                                         esp_ble_mesh_msg_ctx_t *ctx,
                                         esp_ble_mesh_server_recv_light_hsl_set_t *set)
{
    esp_ble_mesh_light_hsl_srv_t *srv = model->user_data;

    switch (ctx->recv_op) {
    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_GET:
        esp_ble_mesh_server_model_send_msg(model, ctx,
            ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->hue), &srv->state->hue);

            esp_ble_mesh_server_model_send_msg(model, ctx,
            ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->saturation), &srv->state->saturation);

            esp_ble_mesh_server_model_send_msg(model, ctx,
            ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->lightness), &srv->state->lightness);
            
        break;
    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
        if (set->op_en == false) {
            srv->state->hue = set->hue;
            srv->state->saturation = set->saturation;
            srv->state->lightness = set->lightness;
        } else {
            /* TODO: Delay and state transition */
           
            srv->state->hue = set->hue;
            srv->state->saturation = set->saturation;
            srv->state->lightness = set->lightness;


        }
        if (ctx->recv_op == ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET) {
            esp_ble_mesh_server_model_send_msg(model, ctx,
                ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->hue), &srv->state->hue);

                esp_ble_mesh_server_model_send_msg(model, ctx,
                ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->saturation), &srv->state->saturation);

                esp_ble_mesh_server_model_send_msg(model, ctx,
                ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS, sizeof(srv->state->lightness), &srv->state->lightness);
        }


        esp_ble_mesh_model_publish(model, ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS,
            sizeof(srv->state->hue), &srv->state->hue, ROLE_NODE);

                   esp_ble_mesh_model_publish(model, ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS,
            sizeof(srv->state->saturation), &srv->state->saturation, ROLE_NODE);

                               esp_ble_mesh_model_publish(model, ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS,
            sizeof(srv->state->lightness), &srv->state->lightness, ROLE_NODE);
       // example_change_led_state(model, ctx, srv->state.onoff);
       //example_change_level(model,ctx,srv->state.level);
       example_change_hsl(model,ctx, srv ->state->hue,srv->state -> saturation, srv->state -> lightness);


        break;
    default:
        break;
    }
}



static void example_ble_mesh_lighting_server_cb(esp_ble_mesh_lighting_server_cb_event_t event,
                                                esp_ble_mesh_lighting_server_cb_param_t *param)
{

esp_ble_mesh_light_hsl_state_t state = {0};
//esp_ble_mesh_server_state_value_t state = {0};
 /*esp_ble_mesh_server_state_value_t state = {0};*/   uint16_t lightness;uint16_t hue,saturation;uint16_t hue_min,hue_max,saturation_min,saturation_max,hue_default,
 saturation_default,lightness_default;
    

    ESP_LOGI(HSL_TAG, "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

        switch(event){

           

            case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
                switch (param->ctx.recv_op){

                    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET:
                    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK:
            /* Light HSL Server Model - Primary Element */
                    ESP_LOGI(HSL_TAG, "lightness 0x%04x, hue 0x%04x, saturation 0x%04x",
                    param->value.state_change.hsl_set.lightness,
                    param->value.state_change.hsl_set.hue,
                    param->value.state_change.hsl_set.saturation);
           /* Update bound states */
                    lightness = param->value.state_change.hsl_set.lightness;
                    state.lightness = lightness;
                    hue = param->value.state_change.hsl_set.hue;
                    state.hue = hue;
                    saturation = param->value.state_change.hsl_set.saturation;
                    state.saturation = saturation;

                    ESP_LOGI(HSL_TAG," Before Red = %d, Green = %d, Blue = %d ",red,green,blue);


                    led_strip_hsv2rgb(hue,saturation,lightness,&red,&green,&blue);

                    ESP_LOGI(HSL_TAG," After Red = %d, Green = %d, Blue = %d ",red,green,blue);


                    ESP_ERROR_CHECK(strip->set_pixel(strip, 5, red, green, blue));

                    esp_ble_mesh_server_model_update_state(hsl_server.model, ESP_BLE_MESH_LIGHT_HSL_STATE, &state);

                   
                    
            
                    break;

                    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET:
                    case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK:

                    hue_min = param->value.state_change.hsl_range_set.hue_range_min;
                    state.hue_range_min = hue_min;
                    hue_max = param->value.state_change.hsl_range_set.hue_range_max;
                    state.hue_range_max = hue_max;
                    saturation_min = param->value.state_change.hsl_range_set.saturation_range_min;
                    state.saturation_range_min = saturation_min;
                    saturation_max = param->value.state_change.hsl_range_set.saturation_range_min;
                    state.saturation_range_max = saturation_max;

                   // state1.light_hsl.hue_min = 78;
                    esp_ble_mesh_server_model_update_state(hsl_setup_server.model, ESP_BLE_MESH_LIGHT_HSL_STATE, &state);

                    
                    /* Light Lightness Setup Server Model - Primary Element*/
               /*  ESP_LOGI(TAG, "lightness min 0x%04x, max 0x%04x",
                        param->value.state_change.hsl_range_set.range_min,
                        param->value.state_change.hsl_range_set.range_max);*/
                    break;

                case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET:
                case ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK:
                    hue_default =  param->value.state_change.hsl_default_set.hue ;
                    state.hue_default = hue_default;
                    saturation_default =  param->value.state_change.hsl_default_set.saturation ;
                    state.saturation_default = saturation_default;
                    lightness_default =   param->value.state_change.hsl_default_set.lightness ;
                    state.lightness_default = lightness_default;
                    esp_ble_mesh_server_model_update_state(hsl_setup_server.model, ESP_BLE_MESH_LIGHT_HSL_STATE, &state); break;

                default: break;
                            }           
            default: break;
        }
}


