#
# Script to generate lookup table for anisotropic lighting
#
# Author: Alex V. Boreskoff
#

import PIL, Image, math

								# basic parameters: bitmap size and diffuse and specular powers
size    = 128
diffPow = 1.0
specPow = 30.0

inv  = 1.0 / size;
im   = Image.new ( "RGB", (size, size) )

for i in range (size):
	for j in range (size):
		x = float ( i ) * inv				# as a (t,l)^2
		y = float ( j ) * inv				# as a (t,h)^2

		diff = math.pow ( 1.0 - x, diffPow )
		spec = math.pow ( 1.0 - y, specPow )

		r = int ( 255.0 * diff )
		g = int ( 255.0 * spec )

		im.putpixel ( (i,j), ( r, g, 0 ) )

im.show ()
im.save ( "aniso-table.bmp", "bmp" )
