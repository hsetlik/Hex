/*
  ==============================================================================

    FMOscillator.cpp
    Created: 4 Jun 2021 1:24:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "FMOscillator.h"

std::array<float, TABLESIZE> WTArray::makeArray (WaveType type)
{
    std::array<float, TABLESIZE> arr;
    switch (type)
    {
        case Sine:
        {
            auto dPhase = juce::MathConstants<float>::twoPi / (float)TABLESIZE;
            for (int i = 0; i < TABLESIZE; ++i)
            {
                arr[i] = std::sin (dPhase * (float)i);
            }
            break;
        }
        case Square:
        {
            for (int i = 0; i < TABLESIZE; ++i)
            {
                if (i < (TABLESIZE / 2))
                    arr[i] = -1.0f;
                else
                    arr[i] = 1.0f;
            }
            break;
        }
        case Saw:
        {
            auto dLevel = 2.0f / (float)TABLESIZE;
            for (int i = 0; i < TABLESIZE; ++i)
            {
                arr[i] = -1.0f + (dLevel * i);
            }
            break;
        }
        case Tri:
        {
            auto dLevel = 4.0f / (float)TABLESIZE;
            float level = -1.0f;
            for (int i = 0; i < TABLESIZE; ++i)
            {
                if (i < TABLESIZE / 2)
                    level += dLevel;
                else
                    level -= dLevel;
                arr[i] = level;
            }
            break;
        }
        case Noise:
        {
            auto rand = juce::Random (5);
            for (int i = 0; i < TABLESIZE; ++i)
            {
                auto raw = rand.nextFloat();
                arr[i] = (raw - 0.5f) * 2.0f;
            }
        }
    }
    return arr;
}
//==============================================================================
SineOsc::SineOsc() :
phase (0.0f),
phaseDelta (0.0f),
skew (0.0f),
lowerIdx (0),
upperIdx (0)
{
    auto dPhase = juce::MathConstants<float>::twoPi / (float)TABLESIZE;
    for (int i = 0; i < TABLESIZE; ++i)
    {
        sineData[i] = std::sin (dPhase * (float)i);
    }
}

float SineOsc::getSample (double hz)
{
    if (hz > nyquist)
        hz = nyquist;
    if (hz < 10.0f)
        hz = 10.0f;
    phaseDelta = (float)(hz / sampleRate);
    phase += phaseDelta;
    if (phase >= 1.0f)
        phase -= 1.0f;
    lowerIdx = floor (phase * TABLESIZE);
    skew = (phase * TABLESIZE) - lowerIdx;
    upperIdx = (lowerIdx == TABLESIZE - 1)? 0 : lowerIdx + 1;
    return MathUtil::fLerp (sineData[lowerIdx], sineData[upperIdx], skew);
}
//==============================================================================
AntiAliasOsc::AntiAliasOsc (WaveType type) : phase (0.0f), tablesAdded (0)
{
    auto firstTable = WTArray::makeArray (type);
    for (int i = 0; i < TABLES_PER_FRAME; ++i)
    {
        tables.add (new Wavetable()); //fill the OwnedArray with empty tables
    }
    float fReal[TABLESIZE];
    float fImag[TABLESIZE];
    for (int i = 0; i < TABLESIZE; ++i)
    {
        fReal[i] = 0.0f;
        fImag[i] = firstTable[i];
    }
    MathUtil::fft (TABLESIZE, fReal, fImag);
    createTables (TABLESIZE, fReal, fImag);
}

void AntiAliasOsc::createTables (int tableSize, float *real, float *imag)
{
    int idx;
    // zero DC offset and Nyquist (set first and last samples of each array to zero, in other words)
    real[0] = imag[0] = 0.0f;
    real[tableSize >> 1] = imag[tableSize >> 1] = 0.0f;
    int maxHarmonic = tableSize >> 1;
    const double minVal = 0.000001f;
    while ((fabs (real[maxHarmonic]) + fabs (imag[maxHarmonic]) < minVal) && maxHarmonic)
        --maxHarmonic;
    float topFreq = (float)(2.0f / 3.0f / maxHarmonic); //note:: topFreq is in units of phase fraction per sample, not Hz
    std::vector<float> ar (tableSize);
    std::vector<float> ai (tableSize);
    float scale = 0.0f;
    float lastMinFreq = 0.0f;
    while (maxHarmonic)
    {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < tableSize; idx++)
            ar[idx] = ai[idx] = 0.0f;
        for (idx = 1; idx <= maxHarmonic; idx++)
        {
            ar[idx] = real[idx];
            ai[idx] = imag[idx];
            ar[tableSize - idx] = real[tableSize - idx];
            ai[tableSize - idx] = imag[tableSize - idx];
        }
        // make the wavetable
        scale = makeTable(ar.data(), ai.data(), tableSize, scale, lastMinFreq, topFreq);
        lastMinFreq = topFreq;
        topFreq *= 2.0f;
        maxHarmonic >>= 1;
    }
}

float AntiAliasOsc::makeTable (float *waveReal, float *waveImag, int numSamples, float scale, float bottomFreq, float topFreq)
{
    tables[tablesAdded]->maxFreq = topFreq;
    tables[tablesAdded]->minFreq = bottomFreq;
    MathUtil::fft (numSamples, waveReal, waveImag);
    if (scale == 0.0f)
    {
        // get maximum value to scale to -1 - 1
        double max = 0.0f;
        for (int idx = 0; idx < numSamples; idx++)
        {
            double temp = fabs (waveImag[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0f / max * 0.999f;
        //printf("Table: %d has scale: %f\n", tablesAdded, scale);
    }
    auto minLevel = std::numeric_limits<float>::max();
    auto maxLevel = std::numeric_limits<float>::min();
    for (int i = 0; i < numSamples; ++i)
    {
        tables[tablesAdded]->table[i] = waveImag[i] * scale;
        if (tables[tablesAdded]->table[i] < minLevel)
            minLevel = tables[tablesAdded]->table[i];
        if (tables[tablesAdded]->table[i] > maxLevel)
            maxLevel = tables[tablesAdded]->table[i];
    }
    auto offset = maxLevel + minLevel;
    minLevel = std::numeric_limits<float>::max();
    maxLevel = std::numeric_limits<float>::min();
    for (int i = 0; i < numSamples; ++i)
    {
        tables[tablesAdded]->table[i] -= (offset / 2.0f); //make sure each table has no DC offset
        if (tables[tablesAdded]->table[i] < minLevel)
            minLevel = tables[tablesAdded]->table[i];
        if (tables[tablesAdded]->table[i] > maxLevel)
            maxLevel = tables[tablesAdded]->table[i];
    }
    ++tablesAdded;
    return (float)scale;
}

Wavetable* AntiAliasOsc::tableForHz (double hz)
{
    phaseDelta = (float) hz / sampleRate;
    for (auto table : tables)
    {
        if (table->maxFreq > phaseDelta && table->minFreq <= phaseDelta)
            return table;
    }
    return tables.getLast();
}

float AntiAliasOsc::getSample (double hz)
{
    if (hz > nyquist)
        hz = nyquist;
    if (hz < 10.0f)
        hz = 10.0f;
    phaseDelta = (float)(hz / sampleRate);
    phase += phaseDelta;
    if (phase > 1.0f)
        phase -= 1.0f;
    auto table = tableForHz (hz);
    bottomIndex = floor (phase * TABLESIZE);
    skew = (phase * TABLESIZE) - bottomIndex;
    bSample = table->table[bottomIndex];
    tSample = (bottomIndex == TABLESIZE - 1) ? table->table[0] : table->table[bottomIndex + 1];
    return MathUtil::fLerp (bSample, tSample, skew);
}
//==============================================================================================
HexOsc::HexOsc() :
pOsc (std::make_unique<SineOsc>()),
currentType (Sine)
{
    
}

void HexOsc::setType (WaveType type)
{
    if (currentType != type)
    {
        currentType = type;
        triggerAsyncUpdate();
    }
}

void HexOsc::handleAsyncUpdate()
{
    if (currentType != Sine && currentType != Noise)
        pOsc.reset(new AntiAliasOsc (currentType));
    else if (currentType == Sine)
        pOsc.reset (new SineOsc());
    else
        pOsc.reset (new NoiseOsc());
}
