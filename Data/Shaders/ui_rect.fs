#version 330 core

in vec2 TexCoords;
in vec4 Color;
flat in int TextureIndex;

out vec4 OutColor;

// Is image used for non-batch rendering
uniform sampler2D Samplers[14];

void main()
{
	vec4 SampleColor = vec4(1.0f);
	if(TextureIndex != 255)
	{
		SampleColor = texture2D(Samplers[TextureIndex], TexCoords);
	}

	OutColor = SampleColor * Color;
}