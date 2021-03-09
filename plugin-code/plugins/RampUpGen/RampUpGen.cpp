// PluginRampUpGen.cpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#include "RampUpGen.hpp"
#include "SC_PlugIn.hpp"

static InterfaceTable *ft;

namespace RampUpGen {

RampUpGen::RampUpGen() {

  // Set the UGen's calculation function depending on the rate of the first
  // argument (frequency)
  if (inRate(Frequency) == calc_FullRate) {
    mCalcFunc = make_calc_function<RampUpGen, &RampUpGen::next_a>();

    // Calculate first value
    next_a(1);
  } else {
    mCalcFunc = make_calc_function<RampUpGen, &RampUpGen::next_k>();

    // Calculate first value
    next_k(1);
  };
}

// The guts of our ramp generator
inline float RampUpGen::progressPhasor(float frequency) {
  // Calculate increment value.
  // Double precision is important in phase values
  // because division errors are accumulated as well
  double increment = static_cast<double>(frequency) / sampleRate();

  m_phase += increment;

  const double minvalue = 0.0;
  const double maxvalue = 1.0;

  // Wrap the phasor if it goes beyond the boundaries
  if (m_phase > maxvalue) {
    m_phase = minvalue + (m_phase - maxvalue);
  } else if (m_phase < minvalue) {
    // in case phase is below minimum value
    m_phase = maxvalue - std::fabs(m_phase);
  }

  return m_phase;
}

// Calculation function for audio rate frequency input
void RampUpGen::next_a(int nSamples) {
  const float *frequency = in(Frequency);
  float *outbuf = out(0);

  for (int i = 0; i < nSamples; ++i) {
    outbuf[i] = progressPhasor(frequency[i]);
  }
}

// Calculation function for control rate frequency input
void RampUpGen::next_k(int nSamples) {
  const float frequencyParam = in(Frequency)[0];
  SlopeSignal<float> slopedFrequency =
      makeSlope(frequencyParam, m_frequency_past);
  float *outbuf = out(0);

  for (int i = 0; i < nSamples; ++i) {
    const float freq = slopedFrequency.consume();

    outbuf[i] = progressPhasor(freq);
  }

  // Store final value of frequency slope to be used next time the calculation
  // function runs
  m_frequency_past = slopedFrequency.value;
}
} // namespace RampUpGen

PluginLoad(RampUpGenUGens) {
  // Plugin magic
  ft = inTable;
  registerUnit<RampUpGen::RampUpGen>(ft, "RampUpGen", false);
}
