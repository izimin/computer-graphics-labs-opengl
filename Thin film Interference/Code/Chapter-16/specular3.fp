!!ARBfp1.0
#
# simple specular shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#	  fragment.texcoord [1] - l in tangent space
#     fragment.texcoord [2] - h in tangent space
#
#	  texture [0] - bump map
#	  texture [1] - emission (ambient) map
#	  texture [2] - diffuse map
#     texture [3] - specular (gloss) map
#

ATTRIB	l         = fragment.texcoord [1];
ATTRIB	h         = fragment.texcoord [2];
PARAM	amb       = { 0, 0, 0.5 };
PARAM	one       = 1;
PARAM	two       = 2;
PARAM	shininess = 20;
PARAM	specColor = { 1, 1, 1 };
PARAM	scale     = 0.03;

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

			# compute (n,l) * diffuse
TEX		temp, fragment.texcoord [0], texture [2], 2D;
DP3_SAT	ln.a, l2, n2;
MUL		diffuse, temp, ln.a;					# now diffuse holds diffuse lighting
MUL		diffuse, diffuse, atten.w;				# add distance attenuation

			# compute (n,h) ^ shininess
DP3_SAT	hn.a, n2, h2;							# compute max ( (n,h), 0 )
LG2		hn.a, hn.a;
MUL		hn.a, hn.a, shininess;
EX2		hn.a, hn.a;								# compute max ((h,n), 0) ^ shininess
MUL		color, specColor, hn.a;					# compute gloss*specColor*max((h,n),0)^shininess
TEX		temp, fragment.texcoord [0], texture [3], 2D;
MUL		color, temp, color;						# now color is specular lighting
MUL		color, color, atten.w;					# add distance attenuation

												# get emission
TEX		temp, fragment.texcoord [0], texture [1], 2D;
ADD_SAT	color, color, temp;
ADD_SAT	result.color, color, diffuse;			# return emission + diffuse + specular
#MOV		result.color, color;

END
