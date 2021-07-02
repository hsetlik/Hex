/*
  ==============================================================================

    DebugUtil.cpp
    Created: 13 Jun 2021 3:21:04pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "DebugUtil.h"
std::unique_ptr<DebugSingle> DebugSingle::instance = nullptr;

DebugSingle* DebugSingle::getInstance()
{
    if (instance == nullptr)
    {
        instance.reset (new DebugSingle());
    }
    return instance.get();
}

DebugSingle::~DebugSingle()
{
    for (auto test : tests)
        test->printTest();
}
