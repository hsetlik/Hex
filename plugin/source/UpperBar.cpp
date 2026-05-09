//===================================================
#include "GUI/UpperBar.h"

UpperBar::UpperBar(HexState* s, const String& verString)
    : versionString(verString), loader(s) {
  addAndMakeVisible(loader);
}

void UpperBar::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float xScale = fBounds.getWidth() / 1800.0f;
  const float yScale = fBounds.getHeight() / 100.0f;
}
