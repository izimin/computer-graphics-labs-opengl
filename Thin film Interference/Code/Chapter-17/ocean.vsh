varying	vec3 lt;
varying	vec3 ht;

uniform	vec4	lightPos;
uniform	vec4	eyePos;

void main(void)
{
	vec3	p = vec3      ( gl_ModelViewMatrix * gl_Vertex );			// transformed point to world space
	vec3	v = normalize ( vec3 ( eyePos )   - p );					// vector to the eye

	lt = normalize ( vec3 ( lightPos ) - p );							// vector to light source
	ht = normalize ( lt + v );

	gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord [0] = gl_MultiTexCoord0;
}

