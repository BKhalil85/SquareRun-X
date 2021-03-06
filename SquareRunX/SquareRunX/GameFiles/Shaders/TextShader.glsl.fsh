#version 330 core

in VSH_OUT
{
    vec2 UVCoords;
} FshIn;

uniform sampler2D FontBitmap;
uniform vec4 TextColor;

out vec4 FragColor;

void main()
{
    vec4 SampledColor = vec4(1.0f, 1.0f, 1.0f, texture(FontBitmap, FshIn.UVCoords).r);
    vec4 FinalColor = TextColor * SampledColor;

    if(FinalColor.a == 0.0)
        discard;

    FragColor = FinalColor;
}
