//
// Eroded vertex shader (a'la RenderMan's)
//

varying float	diffuse;
varying float	specular;
varying	vec2	tex;
uniform	vec4	lightPos;
uniform	vec4	eyePos;

void main(void)
{
	vec3	p = vec3      ( gl_ModelViewMatrix * gl_Vertex );			// transformed point to world space
	vec3	l = normalize ( vec3 ( lightPos ) - p );					// vector to light source
	vec3	v = normalize ( vec3 ( eyePos )   - p );					// vector to the eye
	vec3	h = normalize ( l + v );
	vec3	n = normalize ( gl_NormalMatrix * gl_Normal );				// transformed n

	diffuse  = max ( dot ( n, l ), 0.0 );
	specular = pow ( max ( dot ( n, h ), 0.0 ), 100.0 );
	tex      = gl_MultiTexCoord0.xy;
	tex.y   *= 2.0;

	gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
	//gl_TexCoord [0] = gl_MultiTexCoord0;
}

