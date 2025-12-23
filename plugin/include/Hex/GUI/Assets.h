#pragma once
#include "HexHeader.h"
#include "Color.h"

namespace Assets {
enum AssetID { KnobTop, KnobBkgnd, Thumb, PlaybackOn, PlaybackOff };
juce::Image& getImage(const AssetID& id);

enum WaveID { Sine, Tri, Saw, Square, Noise };

juce::Image& getWaveImage(const WaveID& id, bool isOn);
}  // namespace Assets
