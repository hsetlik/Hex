/*
  ==============================================================================

    WaveGraphComponent.h
    Created: 10 Jun 2021 2:37:56pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "Color.h"
#include "Synthesizer.h"
#include "RingBuffer.h"
#include "MathUtil.h"
#define WAVE_RES 256
#define RING_BUFFER_READ_SIZE 256

class WaveGraph :
public juce::OpenGLRenderer,
public juce::Component
{
public:
    WaveGraph(GraphParamSet* params, RingBuffer<float>* rBuffer);
    ~WaveGraph();
    GraphParamSet* const linkedParams;
    RingBuffer<float>* const ringBuffer;
    void paint(juce::Graphics& g) override;
    //! OpenGL overrides
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;
private:
    struct Uniforms
        {
            Uniforms (juce::OpenGLContext& openGLContext, juce::OpenGLShaderProgram& shaderProgram) :
            resolution(std::make_unique<juce::OpenGLShaderProgram::Uniform>(shaderProgram, "resolution")),
            audioSampleData(std::make_unique<juce::OpenGLShaderProgram::Uniform>(shaderProgram, "audioSampleData"))
            {
                resolution.reset (createUniform (openGLContext, shaderProgram, "resolution"));
                audioSampleData.reset (createUniform (openGLContext, shaderProgram, "audioSampleData"));
            }
            ~Uniforms()
            {
                auto* pRes = resolution.get();
                auto* pAudio = audioSampleData.get();
                
                resolution.release();
                audioSampleData.release();
                
                delete pRes;
                delete pAudio;
            }
            std::unique_ptr<juce::OpenGLShaderProgram::Uniform> resolution;
            std::unique_ptr<juce::OpenGLShaderProgram::Uniform> audioSampleData;
        private:
            static juce::OpenGLShaderProgram::Uniform* createUniform(juce::OpenGLContext& openGLContext,
                                                                juce::OpenGLShaderProgram& shaderProgram,
                                                                const char* uniformName)
            {
                if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
                    return nullptr;
                return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
            }
        };
    void createShaders();
    double fundamental;
    juce::Path trace;
    std::array<float, 256> wavePoints;
    //! OpenGL stuff
    juce::OpenGLContext openGLContext;
    GLuint VBO, VAO, EBO;
    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    std::unique_ptr<Uniforms> uniforms;
    const char* vertexShader;
    const char* fragmentShader;
    juce::AudioBuffer<GLfloat> readBuffer;    // Stores data read from ring buffer
    GLfloat visualizationBuffer[RING_BUFFER_READ_SIZE];    // Single channel to visualize
};
