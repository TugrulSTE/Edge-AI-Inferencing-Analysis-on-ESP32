#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "features.h"

#include "Inference.hpp"

static const char *TAG = "app_main";
extern const uint32_t features[4096];

// RGB Data Callback (96x96x3)
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (float)features[offset + i];
    } 
    
    return 0;
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Tüm Tabelalar İçin Tekil Custom ConvNet Başlatılıyor...");
    Inference inf_obj(4096, &raw_feature_get_data);

    inf_obj.run_inference_custom();
    inf_obj.inference_results();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}