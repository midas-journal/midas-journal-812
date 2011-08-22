Thank you for downloading and trying our code.

The arguments are as follows:

1: The file name of the mean image
2: The file name of the principle component image
3: The output file name of the + standard deviation image
4: The output file name of the - standard deviation image
5: The low threshold for the binary image filter (default -1000)
6: The high threshold for the binary image filter (default 0)
7: The eigen value for the principle component (use 1 if you did not normalize the principle component by dividing by the eigenvalue)
8: The number of standard deviations from the mean you would like to view.




an example use of the command line arguments are as follows for displaying .125 standard deviations of the third principle component image

Draw2dasm.exe	"meanImage.mha" "pcImage2.mha" "3+.125sigma.png" "3-.125sigma.png" -1000 0	2.27683e007	.125