// PluginRampUpGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"

namespace RampUpGen {

class RampUpGen : public SCUnit {
public:
    RampUpGen();

private:
    // Calc function
    void next_a(int nSamples);
    void next_k(int nSamples);
	inline float progressPhasor(float frequency);

	enum Inputs { Frequency };
    // Member variables
	double m_phase{0.0};

	// State variables
	float m_frequency_past{0.f};
};

} // namespace RampUpGen
