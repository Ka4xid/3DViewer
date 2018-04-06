#version 130


uniform sampler2D texture;

uniform mat4 tMatrix;
in vec2 Vert_uv;
in vec3 Normal;

in vec3 Pos;
in vec3 V_Pos;

out vec4 outColor;


void main()
{
    float distanceFromLight = distance( tMatrix * vec4(Pos,1) + vec4(V_Pos*10,1) ,
                                       tMatrix * vec4(0,0,0,1) );

    outColor = texture2D(texture, Vert_uv) * vec4( 1/distanceFromLight*10,
                                                   1/distanceFromLight*10,
                                                   1/distanceFromLight*10,
                                                   1 );
}
