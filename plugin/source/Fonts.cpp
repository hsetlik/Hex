#include "GUI/Fonts.h"
#include "FontData.h"

namespace Fonts {
// helper for brevity
static juce::Font getFromBinary(const char* data, int size) {
  juce::Font font(juce::FontOptions(
      juce::Typeface::createSystemTypefaceFor(data, (size_t)size)));
  return font;
}

juce::Font getFont(const FontID& id, float height) {
  static juce::Font fRobotoLight =
      getFromBinary(FontData::RobotoLight_ttf, FontData::RobotoLight_ttfSize);
  static juce::Font fRobotoLightItalic = getFromBinary(
      FontData::RobotoLightItalic_ttf, FontData::RobotoLightItalic_ttfSize);
  static juce::Font fRobotoMediumItalic = getFromBinary(
      FontData::RobotoMediumItalic_ttf, FontData::RobotoMediumItalic_ttfSize);
  static juce::Font fRobotoBlackItalic = getFromBinary(
      FontData::RobotoBlackItalic_ttf, FontData::RobotoBlackItalic_ttfSize);
  static juce::Font fKenyanReg = getFromBinary(
      FontData::KenyanCoffeeRegular_otf, FontData::KenyanCoffeeRegular_otfSize);
  static juce::Font fKenyanBoldItalic =
      getFromBinary(FontData::KenyanCoffeeBoldItalic_otf,
                    FontData::KenyanCoffeeBoldItalic_otfSize);
  switch (id) {
    case RobotoLight:
      return fRobotoLight.withHeight(height);
      break;
    case RobotoLightItalic:
      return fRobotoLightItalic.withHeight(height);
      break;
    case RobotoMediumItalic:
      return fRobotoMediumItalic.withHeight(height);
      break;
    case RobotoBlackItalic:
      return fRobotoBlackItalic.withHeight(height);
      break;
    case KenyanReg:
      return fKenyanReg.withHeight(height);
      break;
    case KenyanBoldItalic:
      return fKenyanBoldItalic.withHeight(height);
      break;
    default:
      jassert(false);
      return fRobotoLight.withHeight(height);
      break;
  }
}
}  // namespace Fonts

//===================================================
