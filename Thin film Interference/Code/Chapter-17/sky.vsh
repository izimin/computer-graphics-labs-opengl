//
// Simple sky vertex shader
//
// Author: Alex V. Boreskoff
//

varying	vec3 l;
varying	vec3 v;

uniform	vec4	lightPos;
uniform	vec4	eyePos;

void main(void)
{
	vec3	p = vec3 ( gl_ModelViewMatrix * gl_Vertex );	// transformed point to world space

	v = vec3 ( eyePos )   - p;								// vector to the eye
	l = vec3 ( lightPos ) - p;								// vector to light source

	gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord [0] = gl_MultiTexCoord0;
}
