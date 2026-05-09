#version 330
in vec3 TexCoords;
out vec4 color;
uniform samplerCube skybox;
uniform float intensidadSkybox;
uniform vec3 tinteSkybox;
void main()
{
	vec4 texturaSkybox = texture(skybox, TexCoords);
	vec3 colorFinal = texturaSkybox.rgb * tinteSkybox * intensidadSkybox;
	color = vec4(colorFinal, texturaSkybox.a);
}
