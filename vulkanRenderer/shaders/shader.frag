#version 450

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;
/*
float X = fract((gl_FragCoord.x/800 - 1.0)*16);
float Y = fract((gl_FragCoord.y/800 - 1.0)*16);
*/
float X = gl_FragCoord.x/800 - 1.0;
float Y = gl_FragCoord.y/800 - 1.0;
float pi = 3.14159;

void main() {
    outColor = vec4(fragColor);
    outColor.x *= (sin(37*pi*(X*X-Y*Y-X*Y)) + 1)/2;
    outColor.y *= (sin(11*pi*(1/(exp(X*X-Y)))) + 1)/2;
    outColor.z *= (sin(13*pi*1/(0.1+X*X+Y*Y)) + 1)/2;
}
