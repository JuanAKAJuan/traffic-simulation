#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/// \class PPMImage
/// A class for reading and writing PPM images.

class PPMImage
{
public:
	PPMImage(void);
	~PPMImage(void);
	int width;  ///< Width of the image
	int height;  ///< Height of the image
	unsigned char* image;  ///< 1D array that stores the image.

	/// Read the PPM image file
	/// It handles two PPM file types: P3 (ASCII) and P6 (Binary).
	/// \param fileName Name of the PPM image file.
	void ReadFile(string fileName);

private:
	char c;   ///< Helper variable to store a character
	char buf[128];   ///< Helper variable to store up to 128 characters.
	ifstream file;   ///< Input file stream for the PPM file.
	void CheckComment();   ///< Check comments in PPM image and move the position indicator to that after the comment. 
};

