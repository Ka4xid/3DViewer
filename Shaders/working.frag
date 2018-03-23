#version 130


uniform sampler2D texture;

in vec2 Vert_uv;
in vec3 Normal;

in vec3 Pos;
in vec3 V_Pos;

out vec4 outColor;


void main()
{
    float distanceFromLight = distance(vec4(Pos + V_Pos*10,1), vec4(0,0,0,1));

    outColor = texture2D(texture, Vert_uv) * vec4(1,
                                                  clamp(distanceFromLight / 10, 0, 1),
                                                  clamp(distanceFromLight / 10, 0, 1),
                                                  1);
}
