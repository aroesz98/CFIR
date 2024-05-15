#include <CFIR.hpp>
#include <string.h>
#include <cmath>

static constexpr float mPI = 3.14159265358979323846f;

static float sinc(const float x)
{
    if (x == 0)
        return 1;

    return sinf(mPI * x) / (mPI * x);
}

CFIR::CFIR(int taps, filterType type, windowType window, float minFrequency, float maxFrequency) : p_Coefficients(nullptr),
                                                                                                   p_Samples(nullptr),
                                                                                                   p_Window(nullptr),
                                                                                                   p_MinFrequency(minFrequency),
                                                                                                   p_MaxFrequency(maxFrequency)

{
    this->p_Index = 0;
    this->p_Taps = taps;

    p_FilterType = type;
    p_WindowType = window;
}

CFIR::~CFIR()
{
    free(p_Samples);
    free(p_Coefficients);
}

uint32_t CFIR::init()
{
    uint32_t result = ResultCodes::RESULT_SUCCESS;

    do
    {
        p_Coefficients = (float *)malloc(this->p_Taps * sizeof(float));
        if (p_Coefficients == nullptr)
        {
            result = ResultCodes::MEMORY_ALLOCATION_ERROR;
            continue;
        }

        p_Samples = (float *)malloc(this->p_Taps * sizeof(float));
        if (p_Samples == nullptr)
        {
            result = ResultCodes::MEMORY_ALLOCATION_ERROR;
            continue;
        }

        p_Window = (float *)malloc(this->p_Taps * sizeof(float));
        if (p_Window == nullptr)
        {
            result = ResultCodes::MEMORY_ALLOCATION_ERROR;
            continue;
        }

        memset(p_Coefficients, 0, this->p_Taps * sizeof(float));
        memset(p_Samples, 0, this->p_Taps * sizeof(float));
        memset(p_Window, 0, this->p_Taps * sizeof(float));

        // Calc coefficients corresponding to the filter type
        switch (p_FilterType)
        {
            case filterType::LOWPASS_FILTER:
                result = lowPassCoefficients(p_MinFrequency);
            break;

            case filterType::HIGHPASS_FILTER:
                result = highPassCoefficients(p_MinFrequency);
            break;

            case filterType::BANDPASS_FILTER:
                result = bandPassCoefficients(p_MinFrequency, p_MaxFrequency);
            break;

            case filterType::BANDSTOP_FILTER:
                result = bandStopCoefficients(p_MinFrequency, p_MaxFrequency);
            break;
        }

        if (result != ResultCodes::RESULT_SUCCESS)
        {
            continue;
        }

        switch (p_WindowType)
        {
        case windowType::HAMMING:
            windowHamming();
            break;

        case windowType::TRIANGLE:
            windowTriangle();
            break;

        case windowType::BLACKMAN:
            windowBlackman();
            break;
        }

        for (int n = 0; n < this->p_Taps; n++)
        {
            this->p_Coefficients[n] = p_Coefficients[n] * p_Window[n];
        }

        free(p_Window);
    } while (0);

    return result;
}

float *CFIR::getCoefficients(void)
{
    return this->p_Coefficients;
}

uint32_t CFIR::lowPassCoefficients(float frequency)
{
    uint32_t result = ResultCodes::RESULT_SUCCESS;
    int32_t *n = (int32_t *)malloc((this->p_Taps) * sizeof(int32_t));
    if (n == nullptr)
    {
        result = ResultCodes::MEMORY_ALLOCATION_ERROR;
    }

    memset(n, 0, this->p_Taps * sizeof(int32_t));

    for (int i = 0; i < p_Taps; i++)
    {
        n[i] = i - int(p_Taps / 2);
    }

    for (int i = 0; i < p_Taps; i++)
    {
        this->p_Coefficients[i] = 2.0f * frequency * sinc(2.0f * frequency * n[i]);
    }

    free(n);

    return result;
}

uint32_t CFIR::highPassCoefficients(float frequency)
{
    uint32_t result = ResultCodes::RESULT_SUCCESS;
    int32_t *n = (int32_t *)malloc((this->p_Taps) * sizeof(int32_t));
    if (n == nullptr)
    {
        result = ResultCodes::MEMORY_ALLOCATION_ERROR;
    }

    memset(n, 0, this->p_Taps * sizeof(int32_t));

    for (int i = 0; i < p_Taps; i++)
    {
        n[i] = i - int(p_Taps / 2);
    }

    for (int i = 0; i < p_Taps; i++)
    {
        this->p_Coefficients[i] = sinc(n[i]) - 2.0f * frequency * sinc(2.0f * frequency * n[i]);
    }

    free(n);

    return result;
}

uint32_t CFIR::bandPassCoefficients(float minFrequency, float maxFrequency)
{
    uint32_t result = ResultCodes::RESULT_SUCCESS;
    int32_t *n = (int32_t *)malloc((this->p_Taps) * sizeof(int32_t));
    if (n == nullptr)
    {
        result = ResultCodes::MEMORY_ALLOCATION_ERROR;
    }

    memset(n, 0, this->p_Taps * sizeof(int32_t));

    for (int i = 0; i < p_Taps; i++)
    {
        n[i] = i - int(p_Taps / 2);
    }

    for (int i = 0; i < p_Taps; i++)
    {
        this->p_Coefficients[i] = 2.0f * minFrequency * sinc(2.0f * minFrequency * n[i]) - 2.0f * maxFrequency * sinc(2.0f * maxFrequency * n[i]);
    }

    free(n);

    return result;
}

uint32_t CFIR::bandStopCoefficients(float minFrequency, float maxFrequency)
{
    uint32_t result = ResultCodes::RESULT_SUCCESS;
    int32_t *n = (int32_t *)malloc((this->p_Taps) * sizeof(int32_t));
    if (n == nullptr)
    {
        result = ResultCodes::MEMORY_ALLOCATION_ERROR;
    }

    memset(n, 0, this->p_Taps * sizeof(int32_t));

    for (int i = 0; i < p_Taps; i++)
    {
        n[i] = i - int(p_Taps / 2);
    }

    for (int i = 0; i < p_Taps; i++)
    {
        this->p_Coefficients[i] = 2.0f * minFrequency * sinc(2.0f * minFrequency * n[i]) - 2.0f * maxFrequency * sinc(2.0f * maxFrequency * n[i]) + sinc(n[i]);
    }

    free(n);

    return result;
}

void CFIR::windowHamming(void)
{
    float alpha = 0.54f;
    float beta = 0.46f;

    for (int i = 0; i < p_Taps; i++)
    {
        p_Window[i] = alpha - beta * cosf(2.0f * mPI * i / (p_Taps - 1));
    }
}

void CFIR::windowTriangle(void)
{
    float l = this->p_Taps;

    for (int i = 0; i < p_Taps; i++)
    {
        p_Window[i] = 1 - abs((i - (((float)(p_Taps - 1)) / 2.0f)) / (((float)l) / 2.0f));
    }
}

void CFIR::windowBlackman(void)
{
    float alpha0 = 0.42f;
    float alpha1 = 0.5f;
    float alpha2 = 0.08f;

    for (int i = 0; i < this->p_Taps; i++)
    {
        p_Window[i] = alpha0 - alpha1 * cosf(2.0f * mPI * i / (this->p_Taps - 1)) - alpha2 * cosf(4.0f * mPI * i / (this->p_Taps - 1));
    }
}

float CFIR::filter(float new_sample)
{
    uint32_t len = this->p_Taps;
    uint32_t currentIdx = 0;
    float output_sample = 0;

    // Store new sample
    this->p_Samples[this->p_Index] = new_sample;

    // Calc output
    while (len > 31)
    {
        output_sample += this->p_Samples[(this->p_Index + currentIdx) % this->p_Taps] * this->p_Coefficients[currentIdx];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 1) % this->p_Taps] * this->p_Coefficients[currentIdx + 1];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 2) % this->p_Taps] * this->p_Coefficients[currentIdx + 2];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 3) % this->p_Taps] * this->p_Coefficients[currentIdx + 3];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 4) % this->p_Taps] * this->p_Coefficients[currentIdx + 4];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 5) % this->p_Taps] * this->p_Coefficients[currentIdx + 5];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 6) % this->p_Taps] * this->p_Coefficients[currentIdx + 6];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 7) % this->p_Taps] * this->p_Coefficients[currentIdx + 7];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 8) % this->p_Taps] * this->p_Coefficients[currentIdx + 8];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 9) % this->p_Taps] * this->p_Coefficients[currentIdx + 9];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 10) % this->p_Taps] * this->p_Coefficients[currentIdx + 10];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 11) % this->p_Taps] * this->p_Coefficients[currentIdx + 11];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 12) % this->p_Taps] * this->p_Coefficients[currentIdx + 12];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 13) % this->p_Taps] * this->p_Coefficients[currentIdx + 13];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 14) % this->p_Taps] * this->p_Coefficients[currentIdx + 14];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 15) % this->p_Taps] * this->p_Coefficients[currentIdx + 15];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 16) % this->p_Taps] * this->p_Coefficients[currentIdx + 16];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 17) % this->p_Taps] * this->p_Coefficients[currentIdx + 17];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 18) % this->p_Taps] * this->p_Coefficients[currentIdx + 18];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 19) % this->p_Taps] * this->p_Coefficients[currentIdx + 19];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 20) % this->p_Taps] * this->p_Coefficients[currentIdx + 20];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 21) % this->p_Taps] * this->p_Coefficients[currentIdx + 21];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 22) % this->p_Taps] * this->p_Coefficients[currentIdx + 22];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 23) % this->p_Taps] * this->p_Coefficients[currentIdx + 23];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 24) % this->p_Taps] * this->p_Coefficients[currentIdx + 24];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 25) % this->p_Taps] * this->p_Coefficients[currentIdx + 25];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 26) % this->p_Taps] * this->p_Coefficients[currentIdx + 26];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 27) % this->p_Taps] * this->p_Coefficients[currentIdx + 27];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 28) % this->p_Taps] * this->p_Coefficients[currentIdx + 28];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 29) % this->p_Taps] * this->p_Coefficients[currentIdx + 29];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 30) % this->p_Taps] * this->p_Coefficients[currentIdx + 30];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 31) % this->p_Taps] * this->p_Coefficients[currentIdx + 31];
        currentIdx += 32;
        len -= 32;
    }
    while (len > 3)
    {
        output_sample += this->p_Samples[(this->p_Index + currentIdx) % this->p_Taps] * this->p_Coefficients[currentIdx];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 1) % this->p_Taps] * this->p_Coefficients[currentIdx + 1];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 2) % this->p_Taps] * this->p_Coefficients[currentIdx + 2];
        output_sample += this->p_Samples[(this->p_Index + currentIdx + 3) % this->p_Taps] * this->p_Coefficients[currentIdx + 3];
        currentIdx += 4;
        len -= 4;
    }
    while (len > 0)
    {
        output_sample += this->p_Samples[(this->p_Index + currentIdx) % this->p_Taps] * this->p_Coefficients[currentIdx];
        currentIdx++;
        len--;
    }

    // Increase the round robin index
    this->p_Index = (this->p_Index + 1) % this->p_Taps;

    return output_sample;
}
