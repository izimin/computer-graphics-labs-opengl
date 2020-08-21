//
// Vertex shader as a post-processing effect like the one in "Third Space"
//

uniform float	time;

void main(void)
{
	gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord [0] = gl_MultiTexCoord0;
}
