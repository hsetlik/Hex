cmake_minimum_required(VERSION 3.22)

project(Hex VERSION 1.2.0)

set(FONTFILES
"${CMAKE_CURRENT_LIST_DIR}/fonts/Roboto-LightItalic.ttf")

juce_add_binary_data(
  fontBinaries
  HEADER_NAME
  "FontData.h"
  NAMESPACE
  "FontData"
  SOURCES
  "${FONTFILES}")

set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/Hex")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME Setlik # change this
    IS_SYNTH TRUE # may change this
    NEEDS_MIDI_INPUT TRUE # may change this
    NEEDS_MIDI_OUTPUT FALSE # may change this
    PLUGIN_MANUFACTURER_CODE STLK # change this
    PLUGIN_CODE HXFM # change this
    FORMATS Standalone VST3  AU # may change this
    PRODUCT_NAME "Hex" # change this
    COPY_PLUGIN_AFTER_BUILD TRUE 
)

target_sources(${PROJECT_NAME}
PRIVATE
  ${INCLUDE_DIR}/Color.h
  ${INCLUDE_DIR}/ComponentUtil.h
  ${INCLUDE_DIR}/CustomLnF.h
  source/CustomLnF.cpp
  ${INCLUDE_DIR}/DAHDSR.h
  source/DAHDSR.cpp
  ${INCLUDE_DIR}/DebugUtil.h
  source/DebugUtil.cpp
  ${INCLUDE_DIR}/Filter.h
  source/Filter.cpp
  ${INCLUDE_DIR}/FMOperator.h
  source/FMOperator.cpp
  ${INCLUDE_DIR}/FMOscillator.h
  source/FMOscillator.cpp
  ${INCLUDE_DIR}/HexEditor.h
  source/HexEditor.cpp
  ${INCLUDE_DIR}/LFO.h
  source/LFO.cpp
  ${INCLUDE_DIR}/LfoComponent.h
  source/LfoComponent.cpp
  ${INCLUDE_DIR}/MathUtil.h
  source/MathUtil.cpp
  ${INCLUDE_DIR}/ModulationGrid.h
  source/ModulationGrid.cpp
  ${INCLUDE_DIR}/OperatorComponent.h
  source/OperatorComponent.cpp
  ${INCLUDE_DIR}/ParameterLayout.h
  source/ParameterLayout.cpp
  ${INCLUDE_DIR}/PatchBrowser.h
  source/PatchBrowser.cpp
  ${INCLUDE_DIR}/PluginEditor.h
  source/PluginEditor.cpp
  ${INCLUDE_DIR}/PluginProcessor.h
  source/PluginProcessor.cpp
  ${INCLUDE_DIR}/SliderLabel.h
  source/SliderLabel.cpp
  ${INCLUDE_DIR}/SymbolButton.h
  source/SymbolButton.cpp
  ${INCLUDE_DIR}/Synthesizer.h
  source/Synthesizer.cpp
  ${INCLUDE_DIR}/BitmapWaveGraph.h
  source/BitmapWaveGraph.cpp
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Hex
)


# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
    fontBinaries
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_dsp
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
)

juce_generate_juce_header(${PROJECT_NAME})

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-unused-variable -Wno-unused-private-field)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
