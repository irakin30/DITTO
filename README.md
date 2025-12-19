# DITTO - Digital Image Tagging and Trading-Card Observer 

![Yippee!](images/README_ASSETS/ditto.jpg)

### Group Members: Sam Lin, Devin Chen, Istab Rakin 

> [!WARNING] 
> This project is currently a work in progress and thus many features are either
> in an incomplete or broken state.  

## Build Instructions 

> [!IMPORTANT] 
> This project depends on OpenCV and that library must be installed on your
> computer to compile this project 

### Dependencies
```sh
Cmake
Make 
OpenCV # Must be accessible to CMAKE
``` 
 
### Compiling 

In the main directory (the directory with the CMakelists.txt) run: 

```sh
cmake . # creates a makefile for the project
make 
```

This would then output the binary `DITTO` which can then be run.

### Running the project 
> [!NOTE] 
> `input_image` is the image used to grade 
> 
> template_image is used as a placeholder, but still needs to be implemented, 
> thus just use the same image twice as 
> 
> `./DITTO input_image input_image`
> 

```sh 
./DITTO <input_image> <template_image>
```

## Short Introduction 
Ditto is inspired by TAG grading. It is a trading card evaluation company that 
evaluates the quality of Pokémon cards and many different types of trading 
cards. The purpose of this project is to create a replica of TAG’s grading 
process and allow users to scan their own cards before evaluations through TAG 
by allowing the consumer to take a picture of their card, and through some
computer vision algorithms, detect and trace defects to output a score on a 
scale of 1.0 through 10.0 (with 10 being the best). 

## Caveats 
Since Image Tagging usually requires a ring light and multiple cameras at 
different angles, we physically won't have as much data to work off of. So, this
would be a proof-of-concept for now (can we make a tagging system using easily 
accessible hardware) and fine-tune it to an acceptable level? 


## Detecting the Card

In order to actually `TAG` the card, we do have to get a clean image of the card
itself. We initially set a standard card resolution of 330 by 440 pixels. 

So from the original image we create: 
- A greyscale image (we use this as a base, as this is easier to work with). 
- A blurred image (with gaussian to reduce noise) 
- An edged image (an image that determines the edges) 
- A Warped image (the corrected image of the card).

![Greyscale Image, Edges, Warped Image](images/README_ASSETS/preview.png)

![Warped (and corrected) Image](images/README_ASSETS/warp.png) 

## Thresholding and Finding Deformities 

> [!WARNING] 
> This is currently in progress and will be done with some refactoring of the
> code. As of right now, the methodology stays the same. 

So, after we get the fixed image of just the card, we attempt to find major
deformities within the card, such as scratches, bends, dents, and fraying. 
This is done by blurring the image with a Gaussian mask. Next, we find the 
gradient magnitude for edge detection. From this, we will threshold the Gradient Magnitude with the **n** percentile of max threshold.

Afterwards, we filter out large areas within, as these usually present the pokeballs, edges, or giant text

Lastly, we set a Hough transform threshold to detect lines with certain 
amount of votes. 

Very lastly we filter out lines that are of bigger than a certain area size. 


> [!CAUTION] 
> A lot of this remains unimplemented, but here are some images of the progress 
> we have made 

![Incomlpete Bend Detection](images/README_ASSETS/incomplete.png) 

## Centering
The point of centering is to evaluate the alignment of the card and compare it to the template.

## Grading
The score is a weighted combination of three factors:
1. **Template Matching (40%)**  
   Measures overall similarity to the template image using normalized cross-correlation.
2. **Center Offset (40%)**  
   Measures how far the object's center deviates from the card center.
3. **Edge Alignment (20%)**  
   Compares how well the card's edges match the template.

## Algorithms
1. **Template Matching**
- Convert both input images to grayscale
- Calculate the normalized cross-correlation between the card image and the template
- Get the maximum correlation value and convert to a deduction score

2. **Centering Offset**
- Apply binary thresholding to the warped image
- Find all exterior contours
- Select the contour with the largest area
- Compute the bounding rectangle and its center point
- Compute the image center point
- Calculate normalized offsets in both the x and y directions
- Compute the Euclidean distance between centers

3. **Edge Alignment**
- Apply Canny edge detection to both grayscale images
- Compute the absolute difference between the two edge maps
- Count the number of differing pixels
- Divide by the total number of pixels

## Score
- **Lower scores** indicate better centering.
- **0.0** represents the best alignment.
- **2.0** represents the worst alignment.

## Grading System
The grading system is based on a weighted scale. Each card starts with a grade of 10.0 
and deductions are made based on the scratches and centering.

Each card has meta information stored, which includes:
- **Scratch pixel count**
- **Centering deduction**  
- **Final calculated grade**

### Weight Distribution
The final grade is determined using the following weights:
| Factor | Weight |
|--------|--------|
| Scratches | 80% |
| Centering | 20% |

### Condition Category
The card's condition is determined by its final grade:

| Final Grade | Condition |
|-------------|-----------|
| ≥ 9.5 | **Mint** |
| ≥ 9.0 | **Near Mint** |
| ≥ 8.0 | **Excellent** |
| ≥ 7.0 | **Very Good** |
| ≥ 6.0 | **Good** |
| ≥ 5.0 | **Fair** |
| < 5.0 | **Poor** |

![Grading Results](images/README_ASSETS/output.png)

## References
- https://github.com/NolanAmblard/Pokemon-Card-Scanner/tree/main 
- https://github.com/hj3yoo/mtg_card_detector 
- https://www.youtube.com/watch?v=BLy_YF4nmqQ
- https://stackoverflow.com/questions/33227202/detecting-scratch-on-image-with-much-noise
- OpenCV Documentation
