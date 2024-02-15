#include "pch.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#ifndef __clang__
	#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#pragma GCC diagnostic pop

constexpr float negSinF(float x)
{
	float t1 = (x - 3.14159f);
	float increment = t1*t1;
	float t2 = t1*increment;
	float t3 = t2*increment;
	t2 *= -0.166666666667f;
	float t4 = t3*increment;
	t3 *= 0.00833333333333f;
	float t5 = t4*increment;
	t4 *= -0.000198412698413f;
	float t6 = t5*increment;
	t5 *= 2.75573192240e-6f;
	t6 *= -2.50521083854e-8f;
	return t1+t2+t3+t4+t5+t6;
}

//function we are graphing
constexpr float func(float x, float y)
{
	return std::fabs( (y-std::sin(x)) * (std::fmax(std::fabs(x+y)+std::fabs(x-y)-1.0f,0.0f) + std::fmax(0.25f-(x*x)-(y*y),0.0f)) );
}

void threadFunction(unsigned char* imageData, const float& threshold, const size_t& width, const size_t& height, const float& viewWindowTopLeftCornerX, const float& viewWindowTopLeftCornerY, const float& viewWindowBottomRightCornerX, const float& viewWindowBottomRightCornerY, const int& subdivisionResolution, const float& subdivisionThreshold)
{
	//this is a remenant of when every thread was aware of the entire image buffer. I have descided to leave it here incase I ever wish to go back
	const float startX = viewWindowTopLeftCornerX;
	const float startY = viewWindowTopLeftCornerY;

	//pre compute how much the in graph space to move each iteration
	const float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	const float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);
	
	//this is a remenant of when every thread was aware of the entire image buffer. I have descided to leave it here incase I ever wish to go back
	const size_t startIndex = 0;
	//const size_t endIndex = height * width; //unused variable

	//type casting ahead of time
	const size_t subdivisionResolutionSizeT = static_cast<size_t>(subdivisionResolution);
	const float subdivisionResolutionFloat = static_cast<float>(subdivisionResolution);

	//precompute divisions of constants
	const float smallXIncrement = xIncrement / subdivisionResolutionFloat;
	const float smallYIncrement = yIncrement / subdivisionResolutionFloat;

	float x = startX;
	float y = startY;
	
	if(subdivisionResolution == 1)
	{
		size_t i = startIndex;
		for(size_t yi = 0; yi < height; yi++)
		{
			x = startX;
			for(size_t xi = 0; xi < width; xi++)
			{
				//sample the function and perform bit math on it to work out the color
				char showPixle = func(x,y) > threshold;
				imageData[i] = static_cast<unsigned char>(showPixle * 255);

				x += xIncrement;
				i++;
			}
			y -= yIncrement;
		}
	}
	else
	{
		size_t i = startIndex;
		for(size_t yi = 0; yi < height; yi++)
		{
			x = startX;
			for(size_t xi = 0; xi < width; xi++)
			{
				//sample the function
				auto res = func(x,y);
				//if the value is small enough the equality is true
				if(res < threshold)
				{
					
					imageData[i] = 0;
				}
				//if the value is not small enough to be true but also not large enough to discard subdibvide it
				else if(res < subdivisionThreshold)
				{
					//assume the pixle does not contain the value
					imageData[i] = 255;
					
					//same algorithm as for not subdivisions just on a smaller increment
					float newX = x;
					float newY = y;
					for(size_t j = 0; j < subdivisionResolutionSizeT; j++)
					{
						bool shouldBreak = false;
						newX = x;
						for(size_t k = 0; k < subdivisionResolutionSizeT; k++)
						{
							//if we find a sub pixle which satisfies the inequality set the entire pixle to zero
							if(func(newX,newY) < threshold*10)
							{
								imageData[i] = 0;
								shouldBreak = true;
								break;
							}
							newX += smallXIncrement;
						}
						if(shouldBreak) { break; }
						newY -= smallYIncrement;
					}
				}
				//if the value is to large dsicard it
				else
				{
					imageData[i] = 255;
				}

				x += xIncrement;
				i++;
			}
			y -= yIncrement;
		}
	}
}

int main(int argc, char* argv[])
{
	//process command line arguments
	float threshold = 0.01f;
	if(argc >= 2)
	{
		threshold = std::stof(argv[1]);
	}
	float viewWindowTopLeftCornerX = -10.0f;
	float viewWindowTopLeftCornerY = 10.0f;
	float viewWindowBottomRightCornerX = 10.0f;
	float viewWindowBottomRightCornerY = -10.0f;
	if(argc == 3)
	{
		viewWindowTopLeftCornerX = -1.0f * std::stof(argv[2]);
		viewWindowTopLeftCornerY = viewWindowTopLeftCornerX;
		viewWindowBottomRightCornerX = viewWindowTopLeftCornerX;
		viewWindowBottomRightCornerY = -1.0f * viewWindowTopLeftCornerX;
	}
	if(argc >= 6)
	{
		viewWindowTopLeftCornerX = std::stof(argv[2]);
		viewWindowTopLeftCornerY = std::stof(argv[3]);
		viewWindowBottomRightCornerX = std::stof(argv[4]);
		viewWindowBottomRightCornerY = std::stof(argv[5]);
	}
	int width = 1024;
	int height = 1024;
	if(argc == 7)
	{
		width = std::stoi(argv[6]);
		height = width;
	}
	else if(argc >= 8)
	{
		width = std::stoi(argv[6]);
		height = std::stoi(argv[7]);
	}
	int subdivisionResolution = 1;
	float subdivisionThreshold = threshold;
	if(argc >= 10)
	{
		subdivisionResolution = std::stoi(argv[8]);
		subdivisionThreshold = std::stof(argv[9]);
	}
	
	//allocate the image pixle buffer
	unsigned char* imageData = new unsigned char[width * height];
	//std::fill(imageData, imageData + (width * height), 255);
	
	//get the number of threads to work with
	unsigned int numThreads = std::thread::hardware_concurrency();
	if(numThreads == 0) { numThreads = 1; }
	std::thread* threads = new std::thread[numThreads];
	std::cout << "using " << numThreads << " threads" << std::endl;
	
	//pre compute various parameters for the threading process
	const size_t numRowsToProcessPerThread = static_cast<size_t>(height) / numThreads;
	const float viewWindowToProcessPerThread = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(numThreads);
	const size_t pointerOffsetPerThread = static_cast<size_t>(width) * numRowsToProcessPerThread;
	const size_t numRowsToProcessFinalThread = static_cast<size_t>(height) - (numRowsToProcessPerThread * (numThreads - 1));

	//start the timer
	typedef	std::chrono::time_point<std::chrono::high_resolution_clock> TimeType;
	TimeType startTime = std::chrono::high_resolution_clock::now();
	{
		//start the first n-1 threads
		size_t i;
		for(i = 0; i < numThreads-1; i++)
		{
			threads[i] = std::thread(threadFunction, imageData + (i * pointerOffsetPerThread), threshold, width, numRowsToProcessPerThread, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i)), viewWindowBottomRightCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i+1)), subdivisionResolution, subdivisionThreshold);
		}
		//start the last thread. this must be done seperatly as it will also be responsible for processing any rows missed by the truncating nature of integer division
		threads[i] = std::thread(threadFunction, imageData + (i * pointerOffsetPerThread), threshold, width, numRowsToProcessFinalThread, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i)), viewWindowBottomRightCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i+1)), subdivisionResolution, subdivisionThreshold);
	}
	for(size_t i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}
	//end the clock and calculate the time
	TimeType endTime = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();
	auto t2 = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
	std::cout << "Time taken: " << (static_cast<float>(t1 - t2) * 0.001f) << "ms" << std::endl;
	
	//write the image
	stbi_write_png("image.png", width, height, 1, imageData, static_cast<int>(sizeof(char))*width);
	
	delete[] threads;
	delete[] imageData;
	
	return 0;
}
