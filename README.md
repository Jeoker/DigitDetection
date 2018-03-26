# Description: Digit Recognition
### Algorithm: KNN
### Library: OpenCV

This repository use KNN algorithm provided in OpenCV 3.2 to detect printed numbers.

### Training set:
- I used 60 different fonts provided by Microsoft Word as input to train the software.
- I also rotate the samples `[-18, -9, 9, 18]` degrees to increase the size of training set.

### Training Result
- In ideal cases, the accuracy is near 100%.

### Some Details
- Use `alter` to in void_main.cpp to detect black digits on white paper or red LED digits on black board.
- Threshold parameters can be adjusted in Digit_Finder.h for different scenes.
- This software takes only the left digit if several digits are detected.
- It will only confirm the result when a digit is cognized for 8 times continuously.
- The software will quit when 10 digits are detected.
