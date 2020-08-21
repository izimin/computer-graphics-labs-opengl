!!ARBfp1.0
#
# simple edge detector fragment shader
# on entry:
#     fragment.texcoord [0] - texture coordinates
#
#     texture [0] - image to process
#

PARAM	luminance = { 0.3, 0.59, 0.11, 1 };
PARAM	color     = { 1, 0.89, 0.54, 1 };
PARAM	step      = 0.001953125;				# 1/512
PARAM	scale     = 1.2;						# scale factor to enhance edges

TEMP	c, lum;
TEMP	c0, c1, c2, c3;
TEMP	t0, t1, t2, t3;
TEMP	out;

				# compute neighbouring texcoords
MOV	t0, fragment.texcoord [0];
MOV	t1, fragment.texcoord [0];
MOV	t2, fragment.texcoord [0];
MOV	t3, fragment.texcoord [0];

ADD	t0.x, t0, -step;
ADD	t1.x, t1, step;
ADD	t2.y, t2, -step;
ADD	t3.y, t3, step;

				# now fetch values and get intensities
TEX	c0, t0, texture [0], 2D;
DP3	c0.a, c0, luminance;

TEX	c1, t1, texture [0], 2D;
DP3	c1.a, c1, luminance;

TEX	c2, t2, texture [0], 2D;
DP3	c2.a, c2, luminance;

TEX	c3, t3, texture [0], 2D;
DP3	c3.a, c3, luminance;

ADD	out.x, c1.a, -c0.a;
ADD	out.y, c3.a, -c2.a;
ABS	out.xy, out;
ADD	out.x, out.x, out.y;
MUL	out, out, scale;
MOV	out.w, 1;

MOV	result.color, out.x;
END
