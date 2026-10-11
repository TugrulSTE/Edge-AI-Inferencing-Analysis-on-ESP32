#include "Inference.hpp"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "esp_log.h"
#include <cstdio>

struct Inference::Impl
{
    signal_t signal;
    size_t total_length;
    ei_impulse_result_t res = {};
};

Inference::Inference(size_t total_length, GetFeatureCallback user_callback)
{
    impl = new Impl();
    impl->total_length = total_length;
    impl->signal.total_length = total_length;
    impl->signal.get_data = user_callback;
}

Inference::~Inference()
{
    delete impl;
}

void Inference::init()
{
    // init metot gövdesi
    
}

void Inference::run_inference_custom()
{
    EI_IMPULSE_ERROR re = run_classifier(&(impl->signal), &(impl->res), false);
    if (re != EI_IMPULSE_OK)
    {
        printf("Model Çalıştırılamadı! Hata: %d\n\n", re);
        return;
    }
}

void Inference::inference_results()
{
    printf("Çıkarım Tamamlandı! Süre: %d ms\n\n",
           impl->res.timing.dsp + impl->res.timing.classification);

    int top_idx = -1;
    float top_value = 0.0f;

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        printf("[%s]: %.2f%%\n\n",
               impl->res.classification[ix].label,
               impl->res.classification[ix].value * 100.0f);

        if (impl->res.classification[ix].value > top_value)
        {
            top_value = impl->res.classification[ix].value;
            top_idx = ix;
        }
    }

    if (top_idx != -1 && top_value >= 0.50f)
    {
        printf(">>> NİHAİ TABELA: %s (%%%.1f)\n\n",
               impl->res.classification[top_idx].label,
               top_value * 100.0f);
    }
    else
    {
        printf("Sonuç: UNCERTAIN\n\n");
    }
    fflush(stdout);
}