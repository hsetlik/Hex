/*
  ==============================================================================

    HexEditor.cpp
    Created: 7 Jun 2021 2:24:32pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "HexEditor.h"
void SineButton::setSymbol()
{
    const int resolution = 65;
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto y0 = bounds.getY() + (bounds.getHeight() / 2.0f);
    auto amplitude = bounds.getHeight() / 2.0f;
    auto x0 = bounds.getX();
    auto dX = bounds.getWidth() / (float)resolution;
    auto dAngle = juce::MathConstants<float>::twoPi / (float)resolution;
    symbol.clear();
    symbol.startNewSubPath(x0, y0);
    for(int i = 0; i < resolution; ++i)
    {
        auto x = x0 + (i * dX);
        auto y = y0 + (amplitude * std::sin(dAngle * i));
        symbol.lineTo(x, y);
    }
}

void SquareButton::setSymbol()
{
    auto fBounds = getLocalBounds().toFloat();
    auto cushion = fBounds.getHeight() / 8.0f;
    auto bounds = fBounds.reduced(cushion);
    auto dX = bounds.getWidth() / 4;
    auto x0 = bounds.getX();
    auto y0 = bounds.getBottom();
    auto xEnd = bounds.getRight();
    auto yMax = bounds.getY();
    symbol.clear();
    symbol.startNewSubPath(x0, y0);
    symbol.lineTo(x0 + dX, y0);
    symbol.lineTo(x0 + dX, yMax);
    symbol.lineTo(x0 + (3 * dX), yMax);
    symbol.lineTo(x0 + (3 * dX), y0);
    symbol.lineTo(xEnd, y0);
}
