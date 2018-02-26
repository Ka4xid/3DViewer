#version 130

uniform float minHeight;
uniform float maxHeight;
uniform float colorModel;

varying float height;
varying float Color;

varying float Saturation;
varying float Value;

varying vec3 rgb;


void main()
{
    // Vertex shader routine
    vec4 v = vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z, 1);
    gl_Position = gl_ModelViewProjectionMatrix * v;


    // Setting up default colors values
    Saturation = 1;
    Value = 1;

    // Get hight of the point
    height = gl_Vertex.z;

    // NOTE: GLSL have problems of comparing floats!

    // if point lower than minimal height of surface - paint it black
    if ( floor(height) < floor(minHeight) )
    {
        Color = 0;
        Value = 0;
    } else
    // if point higher than maximum height of surface - paint it black
    if (ceil(height) > ceil(maxHeight))
    {
        Color = 0;
        Value = 0;
    } else
    // if point is fine - normalize its height to range 0..1
    {
        Color = 1 - ( (height - minHeight) / (maxHeight - minHeight) );
    }

    // Black & White color model
    if (colorModel == 0) {
        rgb.r = rgb.b = rgb.g = Color;
    } else
    // Heatmap color model
    if (colorModel == 1) {
        float h = Color * 250.f;
        float hh, p, q, t, ff;
        int i;

        hh = h;
        if (hh == 360.0) hh = 0;

        hh /= 60;
        i = int(hh);
        ff = hh - i;
        p = Value * (1 - Saturation);
        q = Value * (1 - (Saturation * ff));
        t = Value * (1 - (Saturation * (1 - ff)));

        if (i==0)
        {
            rgb.r = Value;
            rgb.g = t;
            rgb.b = p;
        } else
        if (i==1)
        {
            rgb.r = q;
            rgb.g = Value;
            rgb.b = p;
        } else
        if (i==2)
        {
            rgb.r = p;
            rgb.g = Value;
            rgb.b = t;
        } else
        if (i==3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = Value;
        } else
        if (i==4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = Value;
        } else
        if (i==5)
        {
            rgb.r = Value;
            rgb.g = p;
            rgb.b = q;
        }
    }

}
