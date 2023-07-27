// PluginRampUpGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "RampUpGen.hpp"
#include "SC_PlugIn.hpp"

static InterfaceTable *ft;

namespace RampUpGen {

RampUpGen::RampUpGen() {

  // Initialize the state of member variables that depend on input aruguments
  m_frequency = in0(Frequency);

  // Set the UGen's calculation function depending on the rate of the first
  // argument (frequency). 
  // Call that function for one calculation cycle, which  generates an 
  // initialization sample for downstream UGens
  if (isAudioRateIn(Frequency)) {
    mCalcFunc = make_calc_function<RampUpGen, &RampUpGen::next_a>();
    next_a(1);
  } else {
    mCalcFunc = make_calc_function<RampUpGen, &RampUpGen::next_k>();
    next_k(1);
  };

  // Reset the initial state of member variables.
  m_phase = 0.0;
  // This is so the initialization sample calulated above by 'next' matches
  // the first output sample when the synth is run and 'next' is called again.
  // m_frequency is not reset because it's initial value is unaffected by 
  // the next_k(1)
}

// The ramp generator
inline float RampUpGen::progressPhasor(double phase, float frequency) {
  // Calculate increment value.
  // Double precision is important in phase values
  // because division errors are accumulated as well
  double increment = static_cast<double>(frequency) / sampleRate();
  
  phase += increment;
  
  const double minvalue = 0.0;
  const double maxvalue = 1.0;

  // Wrap the phasor if it goes above maxvalue or below minvalue
  if (phase > maxvalue) {
    phase = minvalue + (phase - maxvalue);
  } else if (phase < minvalue) {
    phase = maxvalue - std::fabs(phase);
  }

  return phase;
}

// Calculation function for audio rate frequency input
void RampUpGen::next_a(int nSamples) {
  const float *frequency = in(Frequency);
  float *outbuf = out(0);
  double current_phase = m_phase;
  
  for (int i = 0; i < nSamples; ++i) {
    // Be sure to read from UGen's inputs BEFORE writing to outputs, 
    // they share a buffer by default!
    const float freq = frequency[i];
    
    // Write out the phase
    outbuf[i] = current_phase;

    // Advance the phase
    current_phase = progressPhasor(current_phase, freq);
  }

  // Store final value of phase to be used next time the
  // calculation function runs
  m_phase = current_phase;
}

// Calculation function for control rate frequency input
void RampUpGen::next_k(int nSamples) {
  const float frequencyParam = in(Frequency)[0];
  SlopeSignal<float> slopedFrequency =
    makeSlope(frequencyParam, m_frequency);
  float *outbuf = out(0);
  double current_phase = m_phase;

  for (int i = 0; i < nSamples; ++i) {
    const float freq = slopedFrequency.consume();

    // Write out the phase
    outbuf[i] = current_phase;

    // Advance the phase
    current_phase = progressPhasor(current_phase, freq);
  }

  // Store final value of frequency and phase to be used next time the
  // calculation function is called
  m_frequency = slopedFrequency.value;
  m_phase = current_phase;
}

} // namespace RampUpGen

PluginLoad(RampUpGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<RampUpGen::RampUpGen>(ft, "RampUpGen", false);
}
