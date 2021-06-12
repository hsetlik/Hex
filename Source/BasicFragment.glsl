
#version 330 core
uniform vec3  resolution;
uniform float audioSampleData[256];


layout(location = 0) out vec4 diffuseColor;

float getAmplitudeForXPos (in float xPos)
{
// Buffer size - 1
    float perfectSamplePosition = 255.0 * xPos / resolution.x;
    int leftSampleIndex = int (floor (perfectSamplePosition));
    int rightSampleIndex = int (ceil (perfectSamplePosition));
    return (audioSampleData[leftSampleIndex] + audioSampleData[rightSampleIndex]) / 2.0;
}

#define THICKNESS 0.015
void main()
{
    float y = gl_FragCoord.y / resolution.y;
    float amplitude = getAmplitudeForXPos(gl_FragCoord.x);
    //gray: 55, 56, 68
    vec3 gray = vec3(55.0 / 255.0, 56.0 / 255.0, 68.0 / 255.0);
    //highlight: 255, 236, 95
    vec3 highlight = vec3(1.0, 236.0 / 255.0, 95.0 / 255.0);
// Centers & Reduces Wave Amplitude
    amplitude = 0.5 - amplitude / 2.5;
    float r = abs(THICKNESS / (amplitude - y));
    //r * r in the mix function means a sharper falloff from highlight to gray
    vec3 col = mix(gray, highlight, r * r);
    
    diffuseColor = vec4(col, 1.0);
}
