#version 130


uniform sampler2D texture;
varying vec2 vert_texture_coord;
varying vec3 test;

void main()
{
    vec4 color = texture2D(texture, vert_texture_coord);

    gl_FragColor = /*color;*/vec4(vert_texture_coord.x, vert_texture_coord.y, 1, 1);
}
