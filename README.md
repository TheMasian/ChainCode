# ChainCode project
Chain code is loseless compression method for binary images by tracing image contours.

notes:
coordOffset creates a box around a given point to determine the positioning of both the next valid point as well as the position of the last zero before this new point.
the positions are as follows:
```
3 2 1
4 P 0
5 6 7
```
because of this, the first known zero will always start in the 4 position as the image is read from left to right. 

In order to try this code without using the examples data given simply create a txt file with the first line filled with 4 integers representing the number of rows, number of columns, lowest value within the image(which should be 0), highest value within the image(which should range from 1-5 for simplicity)
ex. 20 5 0 1 <--some clarification: the image must be created using either the lowest or highest value thats been set. 

next create a matrix with the previous parameters making any image imaginable, keeping in mind to place a space between each value
ex.
```
0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 1 1 1 1 0 0 0 0 0 1 1 1 0 0 0
0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0
0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0
0 0 0 0 0 0 1 1 1 1 1 1 0 0 0 0 0 0 0 0
```
from here the program will find the first non zero digit within the image and begin the process of outlining the image for compression. This process will present three text files. One file will be an output file that shows the progression of the program working to compress the image. Two more files will be created during the process to provide a chain code and an image boundary created from the compression to be utilized to refill the image in the event of decompression. The chain code file is used to build the boundary file and is read one integer at a time into an array/vector to produce the give image.
