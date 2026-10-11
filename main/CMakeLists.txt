idf_component_register(
    SRCS 
        "main.cpp" "Inference.cpp"
    INCLUDE_DIRS 
        "."
    REQUIRES edge-impulse-sdk freertos esp_timer log
)
target_compile_options(${COMPONENT_LIB} PRIVATE
    -fcommon
    -Wno-error=uninitialized
    -Wno-error=maybe-uninitialized
)