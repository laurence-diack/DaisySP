#include "Utility/dsp.h"
#include "wavetableosc.h"
#include "xtensa/hal.h"

using namespace daisysp;

static inline float Polyblep(float phase_inc, float t);
float* WavetableOsc::sin_table_ = nullptr;

constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

float WavetableOsc::Process()
{
    auto start = xthal_get_ccount();
    int index;
    float out, t;
    switch(waveform_)
    {
        case WAVE_SIN:
            // calculate phase index based on whole wave, fast modulo to table size
            index = (int)(phase_ * TABLE_SIZE / M_PI_2) & (TABLE_SIZE - 1);
            if (phase_ < M_PI_2) // first quarter
            {
                out = sin_table_[index];
            }
            else if (phase_ < PI_F) // second quarter
            {
                out = sin_table_[TABLE_SIZE - index - 1];
            }
            else if (phase_ < PI_F + M_PI_2) // third quarter
            {
                out = -sin_table_[index];
            }
            else // last quarter
            {
                out = -sin_table_[TABLE_SIZE - index - 1];
            }
        break;

        default: out = 0.0f; break;
    }
    phase_ += phase_inc_;
    if(phase_ > TWOPI_F)
    {
        phase_ -= TWOPI_F;
        eoc_ = true;
    }
    else
    {
        eoc_ = false;
    }
    eor_ = (phase_ - phase_inc_ < PI_F && phase_ >= PI_F);
    auto end = xthal_get_ccount();
    printf("proc - %d\n", end - start);
    return out * amp_;
}

float WavetableOsc::CalcPhaseInc(float f)
{
    return (TWOPI_F * f) * sr_recip_;
}

void WavetableOsc::GenerateSinTable()
{
    sin_table_ = new float[TABLE_SIZE]();
    for (int i = 0; i < TABLE_SIZE; i++) {
        sin_table_[i] = sin(M_PI_2 * i / (TABLE_SIZE - 1));
    }

}

static float Polyblep(float phase_inc, float t)
{
    float dt = phase_inc * TWO_PI_RECIP;
    if(t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if(t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    else
    {
        return 0.0f;
    }
}
