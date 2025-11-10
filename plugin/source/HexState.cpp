#include "HexState.h"
#include "Identifiers.h"
#include "ParameterLayout.h"

HexState::HexState(juce::AudioProcessor* proc)
    : mainTree(*proc,
               nullptr,
               ID::HEX_STATE_TREE,
               HexParameters::createLayout()) {
  ValueTree patchTree(ID::HEX_PATCH_INFO);
  patchTree.setProperty(ID::patchName, "Untitled", nullptr);
  patchTree.setProperty(ID::patchAuthor, "User", nullptr);
  patchTree.setProperty(ID::patchType, 0, nullptr);
  mainTree.state.appendChild(patchTree, nullptr);
}
