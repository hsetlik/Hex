#include "GUI/Assets.h"
#include "AssetData.h"

namespace Assets {

juce::Image& getImage(const AssetID& id) {
  static juce::Image iKnobTop = juce::ImageFileFormat::loadFrom(
      AssetData::KnobTop_48x48_png, AssetData::KnobTop_48x48_pngSize);
  static juce::Image iKnobBkgnd = juce::ImageFileFormat::loadFrom(
      AssetData::KnobBkgnd_70x70_png, AssetData::KnobBkgnd_70x70_pngSize);
  static juce::Image iThumb = juce::ImageFileFormat::loadFrom(
      AssetData::Thumb_84x60_png, AssetData::Thumb_84x60_pngSize);
  static juce::Image iPlaybackOn = juce::ImageFileFormat::loadFrom(
      AssetData::PlaybackOn_100x100_png, AssetData::PlaybackOn_100x100_pngSize);
  static juce::Image iPlaybackOff =
      juce::ImageFileFormat::loadFrom(AssetData::PlaybackOff_100x100_png,
                                      AssetData::PlaybackOff_100x100_pngSize);
  switch (id) {
    case KnobTop:
      return iKnobTop;
      break;
    case KnobBkgnd:
      return iKnobBkgnd;
      break;
    case Thumb:
      return iThumb;
      break;
    case PlaybackOn:
      return iPlaybackOn;
      break;
    case PlaybackOff:
      return iPlaybackOff;
      break;
    default:
      jassert(false);
      return iThumb;
  }
}

juce::Image& getWaveImage(const WaveID& id, bool isOn) {
  // on images----
  static juce::Image iSineOn = juce::ImageFileFormat::loadFrom(
      AssetData::SineOn_72x72_png, AssetData::SineOn_72x72_pngSize);
  static juce::Image iTriOn = juce::ImageFileFormat::loadFrom(
      AssetData::TriOn_72x72_png, AssetData::TriOn_72x72_pngSize);
  static juce::Image iSawOn = juce::ImageFileFormat::loadFrom(
      AssetData::SawOn_72x72_png, AssetData::SawOn_72x72_pngSize);
  static juce::Image iSquareOn = juce::ImageFileFormat::loadFrom(
      AssetData::SquareOn_72x72_png, AssetData::SquareOn_72x72_pngSize);
  static juce::Image iNoiseOn = juce::ImageFileFormat::loadFrom(
      AssetData::NoiseOn_72x72_png, AssetData::NoiseOn_72x72_pngSize);
  // off images----
  static juce::Image iSineOff = juce::ImageFileFormat::loadFrom(
      AssetData::SineOff_72x72_png, AssetData::SineOff_72x72_pngSize);
  static juce::Image iTriOff = juce::ImageFileFormat::loadFrom(
      AssetData::TriOff_72x72_png, AssetData::TriOff_72x72_pngSize);
  static juce::Image iSawOff = juce::ImageFileFormat::loadFrom(
      AssetData::SawOff_72x72_png, AssetData::SawOff_72x72_pngSize);
  static juce::Image iSquareOff = juce::ImageFileFormat::loadFrom(
      AssetData::SquareOff_72x72_png, AssetData::SquareOff_72x72_pngSize);
  static juce::Image iNoiseOff = juce::ImageFileFormat::loadFrom(
      AssetData::NoiseOff_72x72_png, AssetData::NoiseOff_72x72_pngSize);
  if (isOn) {
    switch (id) {
      case Sine:
        return iSineOn;
        break;
      case Tri:
        return iTriOn;
        break;
      case Saw:
        return iSawOn;
        break;
      case Square:
        return iSquareOn;
        break;
      case Noise:
        return iNoiseOn;
        break;
      default:
        jassert(false);
        return iSineOn;
    }
  } else {
    switch (id) {
      case Sine:
        return iSineOff;
        break;
      case Tri:
        return iTriOff;
        break;
      case Saw:
        return iSawOff;
        break;
      case Square:
        return iSquareOff;
        break;
      case Noise:
        return iNoiseOff;
        break;
      default:
        jassert(false);
        return iSineOff;
    }
  }
}
}  // namespace Assets
//===================================================
