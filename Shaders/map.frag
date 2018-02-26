#version 130

varying float Color;

varying vec3 rgb;

void main()
{
    gl_FragColor = vec4(rgb, 1);
}
