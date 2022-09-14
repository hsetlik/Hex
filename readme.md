# Hex- FM Synthesis for the Plugin Era

## What is FM synthesis?

[Frequency modulation synthesis](https://en.wikipedia.org/wiki/Frequency_modulation_synthesis) is an audio synthesis technique first developed by John Chowning in the 1960s at Stanford. The technology struggled to garner much interest in the United States until Yamaha licensed Chowning's invention in 1973.
Yamaha's investment in FM would pay off ten years later when 1983's DX7 became one of the most iconic instruments of the decade.

The basic principle of FM is simple enough- if the frequency of one audio-rate sine wave is quickly shifted around based on the amplitude of another sine wave, a wide range of interesting tones and harmonics can be achieved. FM synthesizers expand this concept with the concept of an "opeator". An operator consists of two things

- A waveform at some fraction or multiple of the note's fundamental frequency
- An envelope triggred by each key press which defines the contours of the wave's amplitude
  
Interesting sounds are achieved in FM synthesis by routing one operator to modify the frequency of another by some amount

## Overview

Hex is a flexible FM synthesizer plugin conceived as a more practical and updated take on the FM synthesis algorithms and techniques made famous by instruments like the [Yamaha DX7](https://en.wikipedia.org/wiki/Yamaha_DX7). Despite being behind some of the most iconic synth sounds of the '80s and '90s, the DX7's original design leaves much to be desired. Issues that stuck out to me were primarily

- Only 32 possible modulation routings ('algorithms' in Yamaha parlance)
- Oscillators limited to sine waves
- Limited to 32 saved presets
- Membrane button interface with small LCD makes programming the DX7 a menu-diving nightmare

Today, in the era of DAWs and plugins, the computing power and cost limitations faced by Yamaha's engineers in 1983 have all but disappeared. Hex attempts to let the bedrock concepts of FM synthesis take full advantage of 21st century technology. Notable features include

- Arbitrary modulation routing (any operator can modulate any other operator or itself)
- Plugin GUI including a real-time oscilloscope, operator level meters, and envelope graphs visible at all times
- Oscillators can select one of five wave shapes
- Multi-mode filter with per-voice envelopes for cutoff frequency
- Four LFOs which can modulate any operator's level or the filter's cutoff

## Interface

The plugin's user interface consists of 6 elements

### Operator

Each of the six operators is represented in a panel in the upper left part of the display. In addition to controls and a graph for the DAHDSR envelope, the operator panel contains a wave-shape selector, knob controls for pitch ratio, modulation index, stereo pan, and level, and a button to toggle the operator's audio output.

### Preset Navigator

Preset sounds are saved and loaded via the preset navigator in the upper right corner. Presets can be selected from the dropdown menu or browsed with the next and last preset buttons. The 'save' button opens the window for naming and saving preset patches.

### Modulation Grid

Below the preset navigator is a 6 by 6 grid of buttons which are used to set the modulation routing between operators. Each button is used to toggle a particular carrier/modulator connection.

### Oscilloscope

The oscilloscope sits below the modulation grid and shows a real-time representation of the audio output waveform. The oscilloscope automatically adjusts to the fundamental pitch of a given synth voice such that single wave cycles are always visible.

### Filter

The multi-mode filter has controls for cutoff, resonance, envelope depth, and the wet/dry mix. Like the operators, it has a set of sliders and a graph to control the DAHDSR envelope. The filter cutoff can also be modulated by any one of the four low frequency oscillators.

### Low Frequency Oscillators (LFOs)

Below the operator panels on the left side of the screen are the four LFO control panels. Like the operators, a set of buttons switch between the LFO's five possible wave shapes. Two knobs control the depth of modulation and the LFO's frequency. A "sync" button toggles between frequency control in hertz and frequecy control based on the tempo provided by the plugin host software.

## Design process and programming solutions

Several design considerations and implementation questions went into the development of Hex. These are a few I thought were worth mentioning.

### Oscillator band-limiting

The use of wave shapes other than the pure sine means that we have to consider the upper-frequency harmonic content of the wave. The waves are produced using a wavetable technique, where each sample value is determined by the value at some phase of an array representing the wave shape. When that wave shape has upper harmonics, they too get reproduced relative to the fundamental pitch. When those upper harmonic frequencies get too high, we enounter aliasing. Aliasing occurs when a wavetable is played back at a frequency high enough that some of its harmonics exceed the [Nyquist frequency](https://en.wikipedia.org/wiki/Nyquist_frequency)- which is equal to one half the oscillator's sample rate. For example, if we have a standard 44.1kHz sample rate, any harmonics above 22,050Hz will be reproduced *below* the fundamental frequency rather than above it. This creates the unpleasant distortion we call aliasing.

To combat this, we need to ensure that, at any frequency, no oscillator has harmonics above the Nyquist frequency. Each oscillator has a a set of 10 wavetables; each of which has had all its harmonics above some frequency removed.
The process of generating these tables is the core of the bandlimiting algorithm. Each table goes through this process with its own maximum frequecy.

1. The original wavetable is represented as an array of complex numbers. The imaginary part of the array stores the wave data, while the real part values are all set to zero to start.
2. The complex number arrays are put into a Fast Fourier Transform algorithm to convert the wave from the time domain to the frequency domain.
3. The array members which represent harmonics above the wavetable's designated maximum frequency are set to zero.
4. The complex number arrays are again put into the FFT algorithm to convert the frequency domain representation of the wave back into a discrete time representation.
5. The maximum frequency is divided by two and the next table is created.

Based on its spectral content, each table has a designated frequency range. The function to get the next sample from the oscillator requires a frequency parameter which is needed to determine which table should be used to calculate the output sample.

### Real-time oscilloscope

The oscilloscope in the lower right part of the display is designed to continuously display the waveform at such a scale that individual wave cycles are visible. The OpenGL uniforms which allow the shaders to render the audio data are of a fixed size. The uniform is passed values from a ring buffer owned by the `juce::AudioProcessor` instance. This data is passed from the audio processor to the component-side function that calls the OpenGL instance, we use the `std::atomic` template within a `GraphParamSet` class instance owned by the AudioProcessor. The parameter set contains data for

1. The last level of each operator for each synth voice
2. The index of the last triggered synth voice
3. The number of voices currently active

The function which writes data to the oscilloscope shaders takes a pointer to a `GraphParamSet` and a pointer to the ring buffer.
