# Edge AI Inference Engine on ESP32 & Cross-MCU Benchmark (TinyML)

An on-device machine learning deployment and hardware-level optimization study on the ESP32 platform using the Edge Impulse C++ SDK and ESP-IDF (v5.5). 

This project explores the architectural trade-offs of running 8-bit quantized (INT8) vision models on microcontrollers, featuring a direct benchmark between **Xtensa LX6 (ESP32)** and **ARM Cortex-M4 (STM32G491RE)**, alongside model evolution from MobileNetV1 to a high-accuracy Custom ConvNet.

---

## 📌 Architectural & Low-Level Optimizations

### 1. PIMPL (Pointer to Implementation) Design Pattern
* Isolates Edge Impulse C++ SDK dependencies (`signal_t`, `ei_impulse_result_t`) inside the private implementation layer (`Inference::Impl`).
* Prevents header bloat, decouples the neural network runtime from business logic, and minimizes compilation dependencies across ESP-IDF components.
* Zero dynamic memory overhead during inference via dynamic tensor streaming callbacks (`GetFeatureCallback`).

### 2. ESP32 Hardware-Software Co-Optimization (ESP-IDF v5.5)
* **ESP-NN Vector Acceleration:** Integrated Espressif Neural Network (`ESP-NN`) acceleration via `EI_CLASSIFIER_TFLITE_ENABLE_ESP_NN=1` with `-O3` compiler optimization. Inner-loop 8-bit MAC (Multiply-Accumulate) operations are directly mapped to Xtensa LX6 vector assembly instructions.
* **SPI Flash Bus Tuning (80 MHz QIO):** Reconfigured the external flash bus from the default 40 MHz DIO to **80 MHz QIO (Quad I/O)** via `menuconfig`, quadrupling external memory read bandwidth and eliminating pipeline fetch stalls during inference weights streaming.

---

## 📊 Cross-MCU Hardware Benchmark (MobileNetV1 96×96 INT8)

A comparative evaluation of an identical INT8 quantized model deployed on two distinct microarchitectures demonstrates that clock speed alone does not dictate TinyML throughput:

| Parameter / Metric | ESP32 | STM32G491RE | Architectural Insight |
| :--- | :--- | :--- | :--- |
| **Architecture** | Xtensa LX6 | ARM Cortex-M4 | Dual-core vs. Single-core |
| **Clock Frequency** | 240 MHz | 170 MHz | ESP32 operates at +41.1% higher frequency |
| **Compiler Optimization** | `-O3` | `-O3` | Highest optimization level |
| **Hardware Acceleration** | **ESP-NN** | **CMSIS-NN** (SIMD/DSP) | Microarchitecture-specific vector extensions |
| **Flash Interface** | 80 MHz QIO (External) | Internal Zero-Wait State | External SPI cache misses vs. tight bus coupling |
| **Inference Time (Latency)** | **105.00 ms** | **90.00 ms** | **STM32 is 14.3% faster** |
| **Estimated Clock Cycles** | ~25,200,000 | ~15,300,000 | Cortex-M4 DSP instructions yield higher cycle efficiency |
| **Target Output (Stop)** | 63.67% | 63.67% | Identical quantization consistency |
| **Secondary (Pedestrian)** | 16.02% | 16.02% | Identical quantization consistency |

> **Key Takeaway:** Despite the ESP32's 41.1% raw clock advantage, the STM32G491RE achieves a 14.3% lower inference latency. This highlights the cycle-level efficiency of ARM Cortex-M4 DSP/SIMD instructions combined with internal zero-wait-state flash execution over an external QIO flash bus.

---

## 📈 Model Evolution on ESP32: Latency vs. Accuracy Trade-Off

MobileNetV1 (96×96) provided an ultra-low latency baseline for cross-hardware evaluation, but yielded insufficient confidence for safety-critical traffic sign classification. A Custom ConvNet (64×64) was developed to maximize classification certainty on the edge:

| Model Architecture | Input Shape | Quantization | Latency (ms) | Energy Consumption | Top-1 Confidence (Stop) | Evaluation Context |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **MobileNetV1** | 96 × 96 | INT8 | **105.00 ms** | ~52.50 mJ | 63.67% | Cross-MCU performance baseline |
| **Custom ConvNet** | 64 × 64 | INT8 | **1402.00 ms** | ~729.00 mJ | **93.60%** | Production-ready classification |

* **MobileNetV1:** Prioritizes edge throughput (105 ms / ~9.5 FPS equivalent), suitable for high-speed continuous filtering.
* **Custom ConvNet:** Yields high inference confidence (+29.93% boost) while fitting within the ESP32 internal memory constraints, designed for precision edge validation.

---

## 🛠️ Tech Stack & Toolchain

* **Target Hardware:** Espressif ESP32 Series (Xtensa LX6 @ 240 MHz) & STMicroelectronics STM32G491RE
* **Framework:** ESP-IDF v5.5 (C++17) / CMSIS-NN
* **ML Engines:** Edge Impulse C++ SDK, ESP-NN, TensorFlow Lite Micro
* **Build System:** CMake, Ninja

---

## 🚀 Build and Flash (ESP-IDF)

1. **Set the Target:**
```bash
    idf.py set-target esp32
    idf.py menuconfig
    # Serial flasher config -> Flash SPI speed (80 MHz) -> Flash SPI mode (QIO)
    idf.py build
    idf.py -p <PORT> flash monitor
    ```
