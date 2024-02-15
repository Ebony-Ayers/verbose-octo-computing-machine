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

constexpr float maxF(float a, float b)
{
	return (a * (a>b)) + (b * (a<=b));
}
constexpr float absF(float n)
{
	return (n * (n>0.0f)) + (-n * (n<=0.0f));
}

constexpr float func(float x, float y)
{
	return std::fabs( (y-std::sin(x)) * (std::fmax(std::fabs(x+y)+std::fabs(x-y)-1.0f,0.0f) + std::fmax(0.25f-(x*x)-(y*y),0.0f)) );
	//return absF( (y-std::sin(x)) * (maxF(absF(x+y)+absF(x-y)-1.0f,0.0f) + maxF(0.25f-(x*x)-(y*y),0.0f)) );
}

void threadFunction(unsigned char* imageData, const float& threshold, const size_t& width, const size_t& height, const float& viewWindowTopLeftCornerX, const float& viewWindowTopLeftCornerY, const float& viewWindowBottomRightCornerX, const float& viewWindowBottomRightCornerY, const int& subdivisionResolution, const float& subdivisionThreshold)
{
	const float startX = viewWindowTopLeftCornerX;
	const float startY = viewWindowTopLeftCornerY;

	const float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	const float cumulativeXIncrement = viewWindowBottomRightCornerX - viewWindowTopLeftCornerX;
	const float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);
	
	const size_t startIndex = 0;
	const size_t endIndex = height * width;

	const size_t subdivisionResolutionSizeT = static_cast<size_t>(subdivisionResolution);
	const float subdivisionResolutionFloat = static_cast<float>(subdivisionResolution);

	float x = startX;
	float y = startY;
	
	if(subdivisionResolution == 1)
	{
		for(size_t i = startIndex; i < endIndex; i++)
		{
			char showPixle = func(x,y) > threshold;
			imageData[i] = static_cast<unsigned char>(showPixle * 255);
			
			x += xIncrement;
			x -= cumulativeXIncrement * (i % static_cast<size_t>(width) == 0);
			if(i % static_cast<size_t>(width) == 0)
			{
				x = viewWindowTopLeftCornerX;
			}
			y -= yIncrement * (i % static_cast<size_t>(width) == 0);
		}
	}
	else
	{
		for(size_t i = startIndex; i < endIndex; i++)
		{
			auto res = func(x,y);
			if(res < threshold)
			{
				
				imageData[i] = 0;
			}
			else if(res < subdivisionThreshold)
			{
				imageData[i] = 255;
				float smallXIncrement = xIncrement / subdivisionResolutionFloat;
				float smallYIncrement = yIncrement / subdivisionResolutionFloat;
				
				float newX = x;
				float newY = y;
				for(size_t j = 0; j < subdivisionResolutionSizeT; j++)
				{
					bool shouldBreak = false;
					newX = x;
					for(size_t k = 0; k < subdivisionResolutionSizeT; k++)
					{
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
			else
			{
				imageData[i] = 255;
			}
			
			x += xIncrement;
			x -= cumulativeXIncrement * (i % static_cast<size_t>(width) == 0);
			if(i % static_cast<size_t>(width) == 0)
			{
				x = viewWindowTopLeftCornerX;
			}
			y -= yIncrement * (i % static_cast<size_t>(width) == 0);
		}
	}
}

int main(int argc, char* argv[])
{
	typedef	std::chrono::time_point<std::chrono::high_resolution_clock> TimeType;
	TimeType startTime = std::chrono::high_resolution_clock::now();

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
	
	unsigned char* imageData = new unsigned char[width * height];
	//std::fill(imageData, imageData + (width * height), 255);
	
	unsigned int numThreads = std::thread::hardware_concurrency();
	if(numThreads == 0) { numThreads = 1; }
	std::thread* threads = new std::thread[numThreads];
	std::cout << "using " << numThreads << " threads" << std::endl;
	{
		const size_t numRowsToProcessPerThread = static_cast<size_t>(height) / numThreads;
		const float viewWindowToProcessPerThread = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(numThreads);
		const size_t pointerOffsetPerThread = static_cast<size_t>(width) * numRowsToProcessPerThread;
		size_t i;
		for(i = 0; i < numThreads-1; i++)
		{
			threads[i] = std::thread(threadFunction, imageData + (i * pointerOffsetPerThread), threshold, width, numRowsToProcessPerThread, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i)), viewWindowBottomRightCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i+1)), subdivisionResolution, subdivisionThreshold);
		}
		const size_t numRowsToProcessFinalThread = static_cast<size_t>(height) - (numRowsToProcessPerThread * (numThreads - 1));
		threads[i] = std::thread(threadFunction, imageData + (i * pointerOffsetPerThread), threshold, width, numRowsToProcessFinalThread, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i)), viewWindowBottomRightCornerX, viewWindowTopLeftCornerY - (viewWindowToProcessPerThread * static_cast<float>(i+1)), subdivisionResolution, subdivisionThreshold);
	}
	for(size_t i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}
	delete[] threads;
	
	TimeType endTime = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();
	auto t2 = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
	std::cout << "Time taken: " << (static_cast<float>(t1 - t2) * 0.001f) << "ms" << std::endl;
	
	stbi_write_png("image.png", width, height, 1, imageData, static_cast<int>(sizeof(char))*width);
	
	delete[] imageData;
	
	return 0;
}
