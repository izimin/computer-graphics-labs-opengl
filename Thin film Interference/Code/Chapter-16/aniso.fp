!!ARBfp1.0
#
# simple anisotropic fragment shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#     fragment.texcoord [1] - l in tangent space
#     fragment.texcoord [2] - h in tangent space
#
#     texture [0] - bump map
#     texture [1] - diffuse map
#     texture [2] - specular (gloss) map
#

ATTRIB	l         = fragment.texcoord [1];
ATTRIB	h         = fragment.texcoord [2];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 20;
PARAM	specColor = { 1, 1, 1 };
PARAM	scale     = 0.03;

PARAM	diffusePower  = 1;
PARAM	specularPower = 30;

TEMP	t, t2, dots, color, h2, l2, temp, diffuse, dist, atten;

TEX	t, fragment.texcoord [0], texture [0], 2D;	# get tangent
MAD	t, t, two, -one;
												# normalize n
DP3		dist.w, l, l;
RSQ		l2.w, dist.w;
MUL		l2.xyz, l, l2.w;

												# compute distance attenuation
DP3		h2.w, h, h;
RSQ		h2.w, h2.w;
MUL		h2.xyz, h, h2.w;


DP3	dots.x, l2, t;
DP3	dots.y, h2, t;
MUL	dots.xy, dots, dots;
ADD	dots.xy, one, -dots;
POW	dots.x, dots.x, diffusePower.x;
POW	dots.y, dots.y, specularPower.x;


			# diffuse
TEX		temp, fragment.texcoord [0], texture [1], 2D;
MUL		diffuse, temp, dots.x;					# now diffuse holds diffuse lighting

			# compute (n,h) ^ shininess
MAD_SAT	result.color, specColor, dots.y, diffuse;
#MOV	result.color, diffuse;
END
