file(GLOB_RECURSE SDK_CPP_SRCS "*.cpp")
file(GLOB_RECURSE SDK_C_SRCS "*.c")
file(GLOB_RECURSE SDK_CC_SRCS "*.cc")

idf_component_register(
    SRCS 
        ${SDK_CPP_SRCS}
        ${SDK_C_SRCS}
        ${SDK_CC_SRCS}
    INCLUDE_DIRS 
        "."
        "edge-impulse-sdk"
        "model-parameters"
        "tflite-model"
    REQUIRES freertos esp_timer log espressif__esp-dsp espressif__esp-nn
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    TF_LITE_STATIC_MEMORY
    EI_PORTING_ESPRESSIF=1
    portTICK_RATE_MS=portTICK_PERIOD_MS
    EI_CLASSIFIER_TFLITE_ENABLE_ESP_NN=1
)

# Genel optimizasyon -O2 olarak kalıyor
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error=unused-variable
    -Wno-error=format=
    -Wno-error=deprecated-declarations
    -Wno-deprecated-declarations
    -O2
    -fexceptions
    -Wno-error=uninitialized
    -Wno-error=maybe-uninitialized
    -fcommon
)

# KRİTİK DÜZELTME: Sadece Xtensa GCC'yi çökerten unpack.cc için optimizasyonu düşür
set_source_files_properties(
    "edge-impulse-sdk/tensorflow/lite/micro/kernels/unpack.cc"
    PROPERTIES COMPILE_FLAGS "-O1"
)