#version 330

// Input Variables (received from Vertex Shader)
in vec2 texCoord0;

// Uniform: The Texture
uniform sampler2D texture0;
uniform vec2 resolution = vec2(1280, 720);

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

// Vignette parameters
const float RADIUS = 0.75;
const float SOFTNESS = 0.45;

// Colour definitions
const vec3 lum = vec3(0.299, 0.587, 0.114); // B&W filter
const vec3 sepia = vec3(1.2, 1.0, 0.8);

void main(void)
{
	//color or reset if needed(out at the end)
	outColor = texture(texture0, texCoord0); // for reset just use this

	// Adjust color tint
	vec3 tint = vec3(1.2, 1.0, 0.9); // Slightly warm look
	outColor.rgb *= tint;

	// Increase saturation
	float intensity = dot(outColor.rgb, vec3(0.299, 0.587, 0.114));
	outColor.rgb = mix(vec3(intensity), outColor.rgb, 1.3);
}