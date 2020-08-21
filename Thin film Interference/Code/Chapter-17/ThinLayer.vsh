//
// Thin-layer effect (based on NVidia shader)
//

uniform vec4	lightPos;
uniform vec4	eyePos;

varying vec3	diffColor;
varying vec3	specColor;
varying vec2	viewDepth;

void main()
{
	const	float	filmDepth = 0.4;


    gl_Position = ftransform ();				// transform position to clip space

												// transformed point to world space
	vec3	p = vec3 ( gl_ModelViewMatrix * gl_Vertex );

    											// transform normal from model-space to view-space
    vec3 n = normalize ( gl_NormalMatrix * gl_Normal );

    											// compute the eye->vertex vector
    vec3 v = normalize ( eyePos.xyz - p );

    vec3 l = normalize ( lightPos.xyz - p );
    vec3 h = normalize ( l + v );


    float diffuse = max ( dot ( n, l ), 0.0 );
    float specular = pow ( max ( dot ( n, h ), 0.0 ), 30.0 );

    											// compute the view depth for the thin film
    viewDepth = vec2 ( filmDepth / dot ( n, v ) );

    											// output final lighting results
    diffColor = vec3 ( diffuse  );
    specColor = vec3 ( specular );

				    							// output texture coordinates for diffuse map
    gl_TexCoord [0] = gl_MultiTexCoord0;
}
