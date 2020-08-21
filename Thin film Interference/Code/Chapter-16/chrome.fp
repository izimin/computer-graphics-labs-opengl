!!ARBfp1.0
#
# simple specular shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#	  fragment.texcoord [1] - l in tangent space
#     fragment.texcoord [2] - h in tangent space
#     fragment.texcoord [3] - eye in tangent space
#
#	  texture [0] - bump map
#	  texture [1] - chrome map
#

ATTRIB	l         = fragment.texcoord [1];
ATTRIB	h         = fragment.texcoord [2];
ATTRIB	eye       = fragment.texcoord [3];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 60;
PARAM	scale     = 0.01;
PARAM	specColor = { 1, 1, 1 };

TEMP	n, ln, hn, color, h2, l2, n2, temp, diffuse, dist, atten;

TEX	n, fragment.texcoord [0], texture [0], 2D;	# get normal perturbation
MAD	n, n, two, -one;
												# normalize n
DP3		n2.w, n, n;
RSQ		n2.w, n2.w;
MUL		n2.xyz, n, n2.w;

												# normalize l
DP3		dist.w, l, l;
RSQ		l2.w, dist.w;
MUL		l2.xyz, l, l2.w;

												# compute distance attenuation
MUL		dist.w, dist.w, scale;
SUB_SAT	atten.w, one.w, dist.w;					# as  clamp ( 1 - (l&l) )

												# normalize h
DP3		h2.w, h, h;
RSQ		h2.w, h2.w;
MUL		h2.xyz, h, h2.w;
												# compute diffuse
DP3_SAT	ln.a, l2, n2;
MUL		diffuse, ln.a, atten.w;					# add distance attenuation

												# compute (n,h) ^ shininess
DP3_SAT	hn.a, n2, h2;							# compute max ( (n,h), 0 )
LG2		hn.a, hn.a;
MUL		hn.a, hn.a, shininess;
EX2		hn.a, hn.a;								# compute max ((h,n), 0) ^ shininess
MUL		color, specColor, hn.a;					# now color is specular lighting
MUL		color, color, atten.w;					# add distance attenuation

												# get chrome as chrome [eye]
TEX		temp, fragment.texcoord [3], texture [1], 2D;
MUL		temp, temp, diffuse;
MUL_SAT	color, color, temp;
MUL_SAT	diffuse, diffuse, temp;
ADD_SAT	result.color, color, diffuse;

END
