#ifndef INFERENCE_H
#define INFERENCE_H
#include <cstddef>

typedef int (*GetFeatureCallback)(size_t offset, size_t length, float *out_ptr);

class Inference
{
public:
    Inference(size_t total_length, GetFeatureCallback user_callback);
    ~Inference();
    void init();
    void run_inference_custom();
    void inference_results();

private:
    struct Impl;  
    Impl *impl;
};
#endif