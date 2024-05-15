# CFIR
ARM(CM4/CM7) Optimized FIR filter

Interface:

        uint32_t init(); // Calculate coefficients, allocate memory buffers and configure filter to work

        float* getCoefficients(void);    // Get calculated coefficients
        float filter(float new_sample);  // Filter new sample. It returns filtered sample

Memory usage of compiled library with -O2 switch:
![image](https://github.com/aroesz98/CFIR/assets/87637585/3ef8218f-8d09-4c57-bd16-d2f6e4230dbd)
