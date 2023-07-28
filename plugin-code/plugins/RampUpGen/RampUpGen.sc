RampUpGen : UGen {
	*ar { |frequency=1.0|
		^this.multiNew('audio', frequency);
	}

	*kr { |frequency=1.0|
		^this.multiNew('control', frequency);
	}

	checkInputs {

		// If you want to do custom rate checking...
		if(this.rate == \control and: { inputs.at(0).rate == \audio }, {
			^"An audio-rate frequency argument isn't allowed when RampUpGen runs at control rate."
		});

		// Checks if inputs are valid UGen inputs
		// And not a GUI slider or something...
		^this.checkValidInputs;
	}
}
