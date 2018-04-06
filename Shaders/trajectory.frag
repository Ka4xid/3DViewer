#version 130


uniform sampler2D texture;

in vec2 Vert_uv;
in vec3 Normal;
in float t_time;

out vec4 outColor;


void main()
{
    float Value = abs(sin (t_time*2));
    outColor = vec4(Value, Value, Value, 1);
}
