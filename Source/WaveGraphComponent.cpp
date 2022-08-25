/*
  ==============================================================================

    WaveGraphComponent.cpp
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WaveGraphComponent.h"

WaveGraph::WaveGraph (GraphParamSet* params, RingBuffer<float>* rBuffer) :
linkedParams (params),
ringBuffer (rBuffer),
fundamental (0.0f),
readBuffer (2, RING_BUFFER_READ_SIZE * 5)
{
    openGLContext.setOpenGLVersionRequired (juce::OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext.setRenderer (this);
    openGLContext.attachTo (*getTopLevelComponent());
    openGLContext.setContinuousRepainting (true);
    
    for (int i = 0; i < RING_BUFFER_READ_SIZE; ++i)
    {
        visualizationBuffer[i] = 0.0f; //! fill this array with zeroes to avoid weird graphs on startup
    }
}

WaveGraph::~WaveGraph()
{
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

void WaveGraph::paint (juce::Graphics &g)
{
    
}

void WaveGraph::newOpenGLContextCreated()
{
    // Setup Shaders
    createShaders();
            
    // Setup Buffer Objects
    openGLContext.extensions.glGenBuffers (1, &VBO); // Vertex Buffer Object
    openGLContext.extensions.glGenBuffers (1, &EBO); // Element Buffer Object
}

void WaveGraph::openGLContextClosing()
{
    //shader.release();
    //uniforms.release();
}

void WaveGraph::renderOpenGL()
{
    jassert (juce::OpenGLHelpers::isContextActive());
    int idx = stdu::loadIfLockFree (linkedParams->lastTriggeredVoice);
    
    fundamental = stdu::loadIfLockFree (linkedParams->voiceFundamentals[idx]);
    if (isnan (fundamental))
        fundamental = 1.0f;
    // Setup Viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    juce::gl::glViewport(0, 0, juce::roundToInt(renderingScale * getWidth()), juce::roundToInt (renderingScale * getHeight()));
            
    // Set background Color
    juce::OpenGLHelpers::clear (UXPalette::darkGray);
            
            // Enable Alpha Blending
    juce::gl::glEnable (juce::gl::GL_BLEND);
    juce::gl::glBlendFunc (juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);
            
    // Use Shader Program that's been defined
    shader->use();
    auto samplesPerCycle = (int)44100.0f / fundamental;
    if (fundamental <= 1.0f)
        samplesPerCycle = (int)44100.0f / 440.0f;
    auto sampleIncrement = (samplesPerCycle * 3) / 256;
    
            
            // Setup the Uniforms for use in the Shader
    if (uniforms->resolution != nullptr)
        uniforms->resolution->set ((GLfloat)renderingScale * getWidth(),
                                    (GLfloat) renderingScale * getHeight(),
                                    (GLfloat)samplesPerCycle);
            
            // Read in samples from ring buffer
    if (uniforms->audioSampleData != nullptr)
    {
        ringBuffer->readSamples (readBuffer, RING_BUFFER_READ_SIZE * 5);
                
        juce::FloatVectorOperations::clear (visualizationBuffer, RING_BUFFER_READ_SIZE);
        //fill the visualization buffer with the appropriate samples for the fundamental
        int fallingZeroCross = 0;
        for (int i = 1; i < RING_BUFFER_READ_SIZE * 5; ++i)
        {
            auto sample1 = readBuffer.getSample (0, i - 1);
            auto sample2 = readBuffer.getSample (0, i);
            if (sample1 > 0.0f && sample2 <= 0.0f)
            {
                fallingZeroCross = i;
                break;
            }
        }
        if ((fallingZeroCross + (int)(sampleIncrement * RING_BUFFER_READ_SIZE)) < readBuffer.getNumSamples())
        {
            for (int i = 0; i < RING_BUFFER_READ_SIZE; ++i)
            {
                auto lSample = readBuffer.getSample (0, fallingZeroCross + (int)(sampleIncrement * i));
                auto rSample = readBuffer.getSample (1, fallingZeroCross + (int)(sampleIncrement * i));
                visualizationBuffer[i] = (GLfloat)((lSample + rSample) / 2.0f);
            }
        }
        uniforms->audioSampleData->set (visualizationBuffer, 256);
    }
            
            // Define Vertices for a Square (the view plane)
            GLfloat vertices[] = {
                1.0f,   1.0f,  0.0f,  // Top Right
                1.0f,  -1.0f,  0.0f,  // Bottom Right
                -1.0f, -1.0f,  0.0f,  // Bottom Left
                -1.0f,  1.0f,  0.0f   // Top Left
            };
            // Define Which Vertex Indexes Make the Square
            GLuint indices[] = {  // Note that we start from 0!
                0, 1, 3,   // First Triangle
                1, 2, 3    // Second Triangle
            };
            
            // VBO (Vertex Buffer Object) - Bind and Write to Buffer
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData (juce::gl::GL_ARRAY_BUFFER, sizeof(vertices), vertices, juce::gl::GL_DYNAMIC_DRAW);
            
            // EBO (Element Buffer Object) - Bind and Write to Buffer
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ELEMENT_ARRAY_BUFFER, EBO);
    openGLContext.extensions.glBufferData (juce::gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, juce::gl::GL_DYNAMIC_DRAW);
                                                                        
            
            // Setup Vertex Attributes
    openGLContext.extensions.glVertexAttribPointer (0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            openGLContext.extensions.glEnableVertexAttribArray (0);
        
            // Draw Vertices
    juce::gl::glDrawElements (juce::gl::GL_TRIANGLES, 6, juce::gl::GL_UNSIGNED_INT, 0); // For EBO's (Element Buffer Objects) (Indices)
            
        
            
            // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (juce::gl::GL_ELEMENT_ARRAY_BUFFER, 0);
}

void WaveGraph::createShaders()
{
    juce::String statusText;
    auto shaderProgramAttempt = std::make_unique<juce::OpenGLShaderProgram> (openGLContext);
            
           // Sets up pipeline of shaders and compiles the program
    if (shaderProgramAttempt->addFragmentShader ({BinaryData::BasicFragment_glsl})
            && shaderProgramAttempt->addVertexShader ({BinaryData::BasicVertex_glsl})
               && shaderProgramAttempt->link())
           {
               //uniforms.release();
               shader = std::move (shaderProgramAttempt);
               //shader->setUniform("programID", -1);
               uniforms.reset (new Uniforms (openGLContext, *shader));
               
               statusText = "Using GLSL: v" + juce::String (juce::OpenGLShaderProgram::getLanguageVersion(), 2);
           }
           else
           {
               statusText = shaderProgramAttempt->getLastError();
               
           }
           printf ("%s\n", statusText.toRawUTF8());
}

