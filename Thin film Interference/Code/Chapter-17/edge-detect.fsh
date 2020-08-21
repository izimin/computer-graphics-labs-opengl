//
// Simple edge detection fragment program
//

varying vec2 vUV;
uniform vec2 vDeltaU;
uniform vec2 vDeltaV;
uniform sampler2D oImage;

void main()
{
	vec3 vEdgeU = texture2D(oImage, vUV+vDeltaU) - texture2D(oImage, vUV-vDeltaV);
	vec3 vEdgeV = texture2D(oImage, vUV+vDeltaV) - texture2D(oImage, vUV-vDeltaV);

	gl_FragColor = abs(vEdgeU)+abs(vEdgeV);
}