#pragma once
#ifndef DSY_WAVETABLEOSC_H
#define DSY_WAVETABLEOSC_H
#include <stdint.h>
#include "Utility/dsp.h"
#ifdef __cplusplus

#define TABLE_SIZE 512
#define INTERPOLATION_FACTOR 4

namespace daisysp
{
/** Synthesis of several waveforms, including polyBLEP bandlimited waveforms.
*/
class WavetableOsc
{
  public:
    WavetableOsc() {}
    ~WavetableOsc() {}
    /** Choices for output waveforms, POLYBLEP are appropriately labeled. Others are naive forms.
    */
    enum
    {
        WAVE_SIN,
        WAVE_LAST
    };


    /** Initializes the WavetableOsc

        \param sample_rate - sample rate of the audio engine being run, and the frequency that the Process function will be called.

        Defaults:
        - freq_ = 100 Hz
        - amp_ = 0.5
        - waveform_ = sine wave.
    */
    void Init(float sample_rate)
    {
        sr_        = sample_rate;
        sr_recip_  = 1.0f / sample_rate;
        freq_      = 100.0f;
        amp_       = 0.5f;
        pw_        = 0.5f;
        pw_rad_    = pw_ * TWOPI_F;
        phase_     = 0.0f;
        phase_inc_ = CalcPhaseInc(freq_);
        waveform_  = WAVE_SIN;
        eoc_       = true;
        eor_       = true;

        if (sin_table_ == nullptr)
            GenerateSinTable();
    }


    /** Changes the frequency of the WavetableOsc, and recalculates phase increment.
    */
    inline void SetFreq(const float f)
    {
        freq_      = f;
        phase_inc_ = CalcPhaseInc(f);
    }


    /** Sets the amplitude of the waveform.
    */
    inline void SetAmp(const float a) { amp_ = a; }
    /** Sets the waveform to be synthesized by the Process() function.
    */
    inline void SetWaveform(const uint8_t wf)
    {
        waveform_ = wf < WAVE_LAST ? wf : WAVE_SIN;
    }
    /** Sets the pulse width for WAVE_SQUARE and WAVE_POLYBLEP_SQUARE (range 0 - 1)
     */
    inline void SetPw(const float pw)
    {
        pw_     = fclamp(pw, 0.0f, 1.0f);
        pw_rad_ = pw_ * TWOPI_F;
    }

    /** Returns true if cycle is at end of rise. Set during call to Process.
    */
    inline bool IsEOR() { return eor_; }

    /** Returns true if cycle is at end of cycle. Set during call to Process.
    */
    inline bool IsEOC() { return eoc_; }

    /** Returns true if cycle rising.
    */
    inline bool IsRising() { return phase_ < PI_F; }

    /** Returns true if cycle falling.
    */
    inline bool IsFalling() { return phase_ >= PI_F; }

    inline void PrintWavetable() {
        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            printf("%f\n", sin_table_[i]);
        }
        
    }

    /** Processes the waveform to be generated, returning one sample. This should be called once per sample period.
    */
    float Process();


    /** Adds a value 0.0-1.0 (mapped to 0.0-TWO_PI) to the current phase. Useful for PM and "FM" synthesis.
    */
    void PhaseAdd(float _phase) { phase_ += (_phase * TWOPI_F); }
    /** Resets the phase to the input argument. If no argumeNt is present, it will reset phase to 0.0;
    */
    void Reset(float _phase = 0.0f) { phase_ = _phase; }

  private:
    float   CalcPhaseInc(float f);
    void    GenerateSinTable();
    uint8_t waveform_;
    float   amp_, freq_, pw_, pw_rad_;
    float   sr_, sr_recip_, phase_, phase_inc_;
    float   last_out_, last_freq_;
    bool    eor_, eoc_;
    static float *sin_table_;
};
} // namespace daisysp
#endif
#endif
