!!ARBfp1.0
#
# simple sepia fragment filter
# on entry:
#     fragment.texcoord [0] - texture coordinates
#
#     texture [0] - image to process
#

PARAM	luminance = { 0.3, 0.59, 0.11, 1 };
PARAM	color     = { 1, 0.89, 0.54, 1 };

TEMP	c, lum;

				# get texel
TEX		c, fragment.texcoord [0], texture [0], 2D;

				# get luminance
DP3	lum, c, luminance;
MUL	c.rgb, lum, color;
MOV	c.a, 1;

			# compute (n,h) ^ shininess
MOV	result.color, c;
END
