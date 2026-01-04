
#pragma once
#include "HexHeader.h"

class HexLookAndFeel : public juce::LookAndFeel_V4 {
public:
  //! ComboBox----------------------------------------
  void drawComboBox(juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox& box) override;
  juce::Label* createComboBoxTextBox(juce::ComboBox& box) override;
  juce::Font getComboBoxFont(juce::ComboBox& box) override;
  void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
  // Popup menu----------------------------------------
  void drawPopupMenuItem(juce::Graphics& g,
                         const juce::Rectangle<int>& area,
                         bool isSeparator,
                         bool isActive,
                         bool isHighlighted,
                         bool isTicked,
                         bool hasSubMenu,
                         const String& text,
                         const String& shortcutKeyText,
                         const juce::Drawable* icon,
                         const juce::Colour* textColour) override;
  void drawPopupMenuBackground(juce::Graphics& g,
                               int width,
                               int height) override;
  // Label---------------------------------------------
  juce::Font getLabelFont(juce::Label& label) override;
  void drawLabel(juce::Graphics& g, juce::Label& label) override;
  // Slider--------------------------------------------
  void drawRotarySlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& slider) override;
  void drawLinearSlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float minSliderPos,
                        float maxSliderPos,
                        const juce::Slider::SliderStyle,
                        juce::Slider&) override;
  //! TextButton----------------------------------------
  void drawButtonBackground(juce::Graphics& g,
                            juce::Button& b,
                            const juce::Colour& bColor,
                            bool isHighlighted,
                            bool isDown) override;
  void drawButtonText(juce::Graphics& g,
                      juce::TextButton& b,
                      bool isHighlighted,
                      bool isDown) override;
  int getTextButtonWidthToFitText(juce::TextButton& b, int height) override;
  juce::Font getTextButtonFont(juce::TextButton& b, int height) override;
};
