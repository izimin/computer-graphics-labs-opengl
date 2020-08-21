//
// Lava vertex shader
//

varying	vec3		texCoord;

void main(void)
{
	texCoord     = gl_Vertex.xyz;
	gl_Position  = gl_ModelViewProjectionMatrix * gl_Vertex;
}
