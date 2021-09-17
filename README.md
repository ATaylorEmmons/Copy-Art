# Copy-Art
A program, for my undergraduate thesis in Applied Mathematics, that approximates a given image using rectangles.

# How to Use:
Fill in the params structure in main.  

*populationSize*: Must be even(if its not I don't know what happens). This will cause the program to allocate populationSize Image objects.  
*generationCount*: For how many generation should the program run.  

*eliteRatio*: How many of the elite specimens to preserve
*childRatio*: How many children to be bread from the elites
*randomsRatio*: How many specimens to be randomly made

*colorMutationRate*: How much will a color change if mutation occurs.  
*positionMutationRate*: How much position will change if mutation occurs.(Has poor results)  
*sizeMutationRate*: How much size will change if mutation occurs.(Has poor results)  

*width*: The width of the target image  
*height*: The height of the target image  
*minSize*: The minimum size of a draw square  
*maxSize*: The maximum size of a drawn square  
*rectCount*: How many squares to draw  

*targetImageSrc*: The image to try and replicate  
*saveLocation*: Where to save output images  

# Libraries
For disk io this uses the stb_image libraries found here: https://github.com/nothings/stb.
