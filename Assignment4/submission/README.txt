I talked to Ying Jie for some advice on debugging when the code did not compile as it should.

Currently the code is functional.

Depth filters (min/max depth) is enabled if the argument is included.
Displaying normals is also enabled if the argument is included.

USAGE
a4 -input <txt input_file> -size <width> <height> -output <bmp_output_file> [-depth <min_depth> <max_depth> <bmp_output_depth_file>] [-normal <bmp_normal_output_file>]

examples:
a4 -input scene01_plane.txt -size 200 200 -output 1.bmp -depth 0 10 1depth.bmp -normal 1normal.bmp
a4 -input scene02_cube.txt -size 200 200 -output 2.bmp -depth 0 10 2depth.bmp -normal 2normal.bmp
a4 -input scene03_sphere.txt -size 200 200 -output 3.bmp -depth 0 10 3depth.bmp -normal 3normal.bmp
a4 -input scene04_axes.txt -size 200 200 -output 4.bmp -depth 0 10 4depth.bmp -normal 4normal.bmp
a4 -input scene05_bunny_200.txt -size 200 200 -output 5.bmp -depth 0 10 5depth.bmp -normal 5normal.bmp
a4 -input scene06_bunny_1k.txt -size 1000 1000 -output 6.bmp -depth 0 10 6depth.bmp -normal 6normal.bmp
a4 -input scene07_shine.txt -size 200 200 -output 7.bmp -depth 0 10 7depth.bmp -normal 7normal.bmp
a4 -input scene08_c.txt -size 200 200 -output 8.bmp -depth 0 10 8depth.bmp -normal 8normal.bmp
a4 -input scene09_s.txt -size 200 200 -output 9.bmp -depth 0 10 9depth.bmp -normal 9normal.bmp


scene01_plane.txt
scene02_cube.txt
scene03_sphere.txt
scene04_axes.txt
scene05_bunny_200.txt
scene06_bunny_1k.txt
scene07_shine.txt
scene08_c.txt
scene09_s.txt
