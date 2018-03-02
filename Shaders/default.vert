#version 130


uniform sampler2D texture;
attribute vec3 vert_pos;
attribute vec3 normal;
attribute vec2 texture_coord;
varying vec2 vert_texture_coord;
varying vec3 test;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vert_pos,1);

    vert_texture_coord = texture_coord;
    test = vert_pos;

    test;
}
