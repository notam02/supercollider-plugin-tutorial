// PluginRampUpGen.hpp
// Mads Kjeldgaard (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"

namespace RampUpGen {

class RampUpGen : public SCUnit {
public:
    RampUpGen();

private:
    // Calc functions
    void next_a(int nSamples);
    void next_k(int nSamples);

    // Helper functions
	inline float progressPhasor(double phase, float frequency);

    // Parameter names corresponding to our argument indices
	enum Inputs { Frequency };
    
    // State variables
	double m_phase{0.0};
	float m_frequency; // this will be initialized in the constructor
};

} // namespace RampUpGen
