#version 130


in vec3 Vert_Pos;
in vec3 Vert_Normal;
in vec2 Vert_Texel;
in float time;

out vec2 Vert_uv;
out vec3 Normal;

varying vec4 Pos;


void main()
{
    //Pos = vec4(Vert_Pos.x, Vert_Pos.y, Vert_Pos.z  * (0.5 + (sin(Vert_Pos.x+time)/2)), 1);
    Pos = vec4(Vert_Pos + Vert_Normal*(clamp(sin(time*3), 0, 1)),1 );

    gl_Position = gl_ModelViewProjectionMatrix * Pos;

    Vert_uv = vec2(Vert_Texel);

    Normal = Vert_Normal;
}
