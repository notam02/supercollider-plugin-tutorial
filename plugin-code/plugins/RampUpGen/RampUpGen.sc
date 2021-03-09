RampUpGen : UGen {
	*ar { |frequency=1.0|
		^this.multiNew('audio', frequency);
	}

	*kr { |frequency=1.0|
		^this.multiNew('control', frequency);
	}

	checkInputs {

		// Input 0 is frequency
		if(inputs.at(0) == \audio, {
			"You're not supposed to use audio rate here".error
		});

		// Checks if inputs are valid UGen inputs
		// And not a GUI slider or something...
		^this.checkValidInputs;
	}
}
