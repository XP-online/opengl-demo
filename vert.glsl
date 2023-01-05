#version 450 core
layout (location = 0) in vec4 vPosition;
uniform vec4 ourColor;
void main()
{
  gl_Position = vPosition;
  ourColor = vec4(0.5, 0.0, 0.0, 1.0);
}