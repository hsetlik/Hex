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
readBuffer(2, RING_BUFFER_READ_SIZE)
{
    openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
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

void WaveGraph::handleAsyncUpdate()
{
    wavePoints = linkedParams->dataValues;
    updateTrace();
    repaint();
}

void WaveGraph::updateTrace()
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
            
            // Setup the Uniforms for use in the Shader
            
            if (uniforms->resolution != nullptr)
                uniforms->resolution->set ((GLfloat) renderingScale * getWidth(), (GLfloat) renderingScale * getHeight());
            
            // Read in samples from ring buffer
            if (uniforms->audioSampleData != nullptr)
            {
                ringBuffer->readSamples (readBuffer, RING_BUFFER_READ_SIZE);
                
                juce::FloatVectorOperations::clear (visualizationBuffer, RING_BUFFER_READ_SIZE);
                
                // Sum channels together
                for (int i = 0; i < 2; ++i)
                {
                    juce::FloatVectorOperations::add (visualizationBuffer, readBuffer.getReadPointer(i, 0), RING_BUFFER_READ_SIZE);
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
    vertexShader =
           "attribute vec3 position;\n"
           "\n"
           "void main()\n"
           "{\n"
           "    gl_Position = vec4(position, 1.0);\n"
           "}\n";
           
        fragmentShader =
           "uniform vec2  resolution;\n"
           "uniform float audioSampleData[256];\n"
           "\n"
           "void getAmplitudeForXPos (in float xPos, out float audioAmplitude)\n"
           "{\n"
           // Buffer size - 1
           "   float perfectSamplePosition = 255.0 * xPos / resolution.x;\n"
           "   int leftSampleIndex = int (floor (perfectSamplePosition));\n"
           "   int rightSampleIndex = int (ceil (perfectSamplePosition));\n"
           "   audioAmplitude = mix (audioSampleData[leftSampleIndex], audioSampleData[rightSampleIndex], fract (perfectSamplePosition));\n"
           "}\n"
           "\n"
           "#define THICKNESS 0.02\n"
           "void main()\n"
           "{\n"
           "    float y = gl_FragCoord.y / resolution.y;\n"
           "    float amplitude = 0.0;\n"
           "    getAmplitudeForXPos (gl_FragCoord.x, amplitude);\n"
           "\n"
           // Centers & Reduces Wave Amplitude
           "    amplitude = 0.5 - amplitude / 2.5;\n"
           "    float r = abs (THICKNESS / (amplitude-y));\n"
           "\n"
           "gl_FragColor = vec4 (r - abs (r * 0.2), r - abs (r * 0.2), r - abs (r * 0.2), 1.0);\n"
           "}\n";
            juce::String statusText;
           std::unique_ptr<juce::OpenGLShaderProgram> shaderProgramAttempt = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
            
           // Sets up pipeline of shaders and compiles the program
           if (shaderProgramAttempt->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader))
               && shaderProgramAttempt->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
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
               printf("%s\n", statusText.toRawUTF8());
           }
           
           triggerAsyncUpdate();
}

