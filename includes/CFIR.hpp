#ifndef FIR_HPP
#define FIR_HPP

#include <stdint.h>

class CFIR
{
    public:
        enum ResultCodes
        {
            RESULT_SUCCESS = 0,
            MEMORY_ALLOCATION_ERROR,
        };

        enum filterType
        {
            LOWPASS_FILTER  = 0,
            HIGHPASS_FILTER = 1,
            BANDPASS_FILTER = 2,
            BANDSTOP_FILTER = 3,
        };

        enum windowType
        {
            HAMMING  = 0,
            TRIANGLE = 1,
            BLACKMAN = 2,
        };

        CFIR(int taps = 0, filterType type=filterType::LOWPASS_FILTER, windowType window=windowType::HAMMING, float minFrequency = 0, float maxFrequency = 0);
        ~CFIR();

        uint32_t init();

        float* getCoefficients(void);
        float filter(float new_sample);

    private:
        uint32_t lowPassCoefficients(float frequency);
        uint32_t highPassCoefficients(float frequency);
        uint32_t bandPassCoefficients(float minFrequency, float maxFrequency);
        uint32_t bandStopCoefficients(float minFrequency, float maxFrequency);

        void windowHamming(void);
        void windowTriangle(void);
        void windowBlackman(void);

        float* p_Coefficients;
        float* p_Samples;
        float* p_Window;

        int p_MinFrequency;
        int p_MaxFrequency;
        int p_Index;
        int p_Taps;
        filterType p_FilterType;
        windowType p_WindowType;
};

#endif // FIR_HPP
