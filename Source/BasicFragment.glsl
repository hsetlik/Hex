
#version 330 core
uniform vec3  resolution;
uniform float audioSampleData[256];


layout(location = 0) out vec4 diffuseColor;

void getAmplitudeForXPos (in float xPos, out float audioAmplitude)
{
// Buffer size - 1
    float perfectSamplePosition = 255.0 * xPos / resolution.x;
    int leftSampleIndex = int (floor (perfectSamplePosition));
    int rightSampleIndex = int (ceil (perfectSamplePosition));
    audioAmplitude = (audioSampleData[leftSampleIndex] + audioSampleData[rightSampleIndex]) / 2.0;
}

#define THICKNESS 0.006
void main()
{
    float y = gl_FragCoord.y / resolution.y;
    float amplitude = 0.0;
    getAmplitudeForXPos(gl_FragCoord.x, amplitude);
// Centers & Reduces Wave Amplitude
    amplitude = 0.5 - amplitude / 2.5;
    float r = abs(THICKNESS / (amplitude - y));
    
    diffuseColor = vec4(r - abs(r * 0.2), r - abs(r * 0.2), r, 1.0);
}