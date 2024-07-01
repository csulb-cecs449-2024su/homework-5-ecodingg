#version 330
// A fragment shader for rendering a mesh that has a texture, but no lighting.
layout (location=0) out vec4 FragColor;

// Input from vertices: interpolated texture coordinate.
in vec2 TexCoord;
TexCoord.x = 1-TexCoord.x;
TexCoord.y = 1-TexCoord.y;

// Uniform from application: the texture sampler.
uniform sampler2D baseTexture;

void main() {
    // Sample the texture image at the fragment's texture coordinate.
    FragColor = texture(baseTexture, TexCoord);
}