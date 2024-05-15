# CFIR
ARM(CM4/CM7) Optimized FIR filter

Interface:

        uint32_t init(); // Calculate coefficients, allocate memory buffers and configure filter to work

        float* getCoefficients(void);    // Get calculated coefficients
        float filter(float new_sample);  // Filter new sample. It returns filtered sample
