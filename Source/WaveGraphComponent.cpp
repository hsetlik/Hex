/*
  ==============================================================================

    WaveGraphComponent.cpp
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WaveGraphComponent.h"

WaveGraph::WaveGraph(GraphParamSet* params, RingBuffer<GLfloat>* rBuffer) :
linkedParams(params),
ringBuffer(rBuffer),
fundamental(0.0f),
readBuffer(2, RING_BUFFER_READ_SIZE * 5)
{
    openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*getTopLevelComponent());
    openGLContext.setContinuousRepainting(true);
}

WaveGraph::~WaveGraph()
{
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

void WaveGraph::paint(juce::Graphics &g)
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
    jassert(juce::OpenGLHelpers::isContextActive());
    fundamental = linkedParams->voiceFundamentals[linkedParams->lastTriggeredVoice];
    // Setup Viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    glViewport (0, 0, juce::roundToInt(renderingScale * getWidth()), juce::roundToInt(renderingScale * getHeight()));
            
    // Set background Color
    juce::OpenGLHelpers::clear(UXPalette::darkGray);
            
            // Enable Alpha Blending
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
    // Use Shader Program that's been defined
    shader->use();
    auto samplesPerCycle = (int)44100.0f / fundamental;
    if(fundamental < 1.0f)
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
        for(int i = 1; i < RING_BUFFER_READ_SIZE * 5; ++i)
        {
            auto sample1 = readBuffer.getSample(0, i - 1);
            auto sample2 = readBuffer.getSample(0, i);
            if(sample1 > 0.0f && sample2 <= 0.0f)
            {
                fallingZeroCross = i;
                break;
            }
        }
        if((fallingZeroCross + (int)(sampleIncrement * 256)) < readBuffer.getNumSamples())
        {
            for(int i = 0; i < 256; ++i)
            {
                auto lSample = readBuffer.getSample(0, fallingZeroCross + (int)(sampleIncrement * i));
                auto rSample = readBuffer.getSample(1, fallingZeroCross + (int)(sampleIncrement * i));
                visualizationBuffer[i] = lSample + rSample;
            }
        }
        uniforms->audioSampleData->set(visualizationBuffer, 256);
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
            
            // Vertex Array Object stuff for later
            //openGLContext.extensions.glGenVertexArrays(1, &VAO);
            //openGLContext.extensions.glBindVertexArray(VAO);
            
            // VBO (Vertex Buffer Object) - Bind and Write to Buffer
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
            openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
                                                                        // GL_DYNAMIC_DRAW or GL_STREAM_DRAW
                                                                        // Don't we want GL_DYNAMIC_DRAW since this
                                                                        // vertex data will be changing alot??
                                                                        // test this
            
            // EBO (Element Buffer Object) - Bind and Write to Buffer
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);
            openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
                                                                        // GL_DYNAMIC_DRAW or GL_STREAM_DRAW
                                                                        // Don't we want GL_DYNAMIC_DRAW since this
                                                                        // vertex data will be changing alot??
                                                                        // test this
            
            // Setup Vertex Attributes
            openGLContext.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            openGLContext.extensions.glEnableVertexAttribArray (0);
        
            // Draw Vertices
            //glDrawArrays (GL_TRIANGLES, 0, 6); // For just VBO's (Vertex Buffer Objects)
            glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // For EBO's (Element Buffer Objects) (Indices)
            
        
            
            // Reset the element buffers so child Components draw correctly
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
            //openGLContext.extensions.glBindVertexArray(0);
}

void WaveGraph::createShaders()
{
    juce::String statusText;
    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgramAttempt = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
            
           // Sets up pipeline of shaders and compiles the program
    if(shaderProgramAttempt->addFragmentShader({BinaryData::BasicFragment_glsl})
            && shaderProgramAttempt->addVertexShader({BinaryData::BasicVertex_glsl})
               && shaderProgramAttempt->link())
           {
               uniforms.release();
               shader = std::move(shaderProgramAttempt);
               uniforms.reset(new Uniforms (openGLContext, *shader));
               
               statusText = "GLSL: v" + juce::String(juce::OpenGLShaderProgram::getLanguageVersion(), 2);
           }
           else
           {
               statusText = shaderProgramAttempt->getLastError();
               
           }
           printf("%s\n", statusText.toRawUTF8());
}

