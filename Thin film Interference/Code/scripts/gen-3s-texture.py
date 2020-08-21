#
# script to build texture for "Third Space" effect
#
# Author: Alex V. Boreskoff
#

import noise
import Image
import math

size = 256
inv  = 1.0 / size


im = Image.new ( "RGB", (size, size) )

redOffs   = ( 1.3579, 0.25672, 0.4567 )
greenOffs = ( 2.4786, 1.54567, 2.654  )
blueOffs  = ( 3.5795, 2.16713, 1.0912 )

                    # now build the texture
for x in range ( size ):
    for y in range ( size ):
        x1 = inv * x
        y1 = inv * y

                    # get func value based on noise
        n = ( 1 + noise.noise ( redOffs [0] + 35*x1, redOffs [1] + 4*y1, redOffs [2] ) ) * 0.15;
        v = (y1 + n) * 0.5

        if v < 0:
            v = 1.0

        im.putpixel ( (x,y), (255*v, 0, 0) )

im.show ()
im.save ( "ThirdSpace.bmp", "bmp" )
