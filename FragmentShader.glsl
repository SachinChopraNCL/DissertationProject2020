#version 400 core

uniform sampler2D 	mainTex;
uniform vec3 speed;
in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{	
	if(textureSize(mainTex, 1).x < 1.0f) {
		fragColor = vec4(1,0,0,1);
	}
	else {
		fragColor = texture(mainTex, IN.texCoord) * IN.colour;
	}
}