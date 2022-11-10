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

void threadFunction(unsigned char* imageData, size_t startPixleY, size_t endPixleY, float threshold, int width, int height, float viewWindowTopLeftCornerX, float viewWindowTopLeftCornerY, float viewWindowBottomRightCornerX, float viewWindowBottomRightCornerY, int subdivisionResolution, float subdivisionThreshold)
{
	float startX = viewWindowTopLeftCornerX;
	float startY = viewWindowTopLeftCornerY - ((viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) * (static_cast<float>(startPixleY) / static_cast<float>(width)));

	float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	float cumulativeXIncrement = viewWindowBottomRightCornerX - viewWindowTopLeftCornerX;
	float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);
	
	size_t startIndex = startPixleY * static_cast<size_t>(width);
	size_t endIndex = endPixleY * static_cast<size_t>(width);

	float x = startX;
	float y = startY;
	
	size_t subdivisionResolutionSizeT = static_cast<size_t>(subdivisionResolution);
	float subdivisionResolutionFloat = static_cast<float>(subdivisionResolution);
	
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
void threadFunctionContour(unsigned char* imageData, size_t iStart, size_t iEnd, float threshold, int width, int height, float viewWindowTopLeftCornerX, float viewWindowTopLeftCornerY, float viewWindowBottomRightCornerX, float viewWindowBottomRightCornerY)
{
	size_t startPixleX = iStart % static_cast<size_t>(width);
	size_t startPixleY = iStart / static_cast<size_t>(height);
	float startX = viewWindowTopLeftCornerX + ((viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) * (static_cast<float>(startPixleX) / static_cast<float>(width)));
	float startY = viewWindowTopLeftCornerY - ((viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) * (static_cast<float>(startPixleY) / static_cast<float>(width)));
	
	float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	float cumulativeXIncrement = viewWindowBottomRightCornerX - viewWindowTopLeftCornerX;
	float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);
	
	float x = startX;
	float y = startY;
	for(size_t i = iStart; i < iEnd; i++)
	{
		auto res = func(x,y);
		if(res > (threshold * 524288))
		{
			imageData[i] = 255;
		}
		else if(res > (threshold * 131072))
		{
			imageData[i] = 204;
		}
		else if(res > (threshold * 32768))
		{
			imageData[i] = 153;
		}
		else if(res > (threshold * 8192))
		{
			imageData[i] = 102;
		}
		else if(res > (threshold * 2048))
		{
			imageData[i] = 51;
		}
		else
		{
			imageData[i] = 0;
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

int main(int argc, char* argv[])
{
	typedef	std::chrono::time_point<std::chrono::high_resolution_clock> TimeType;
	TimeType startTime = std::chrono::high_resolution_clock::now();

	bool showContours = false;
	if(argc >= 2)
	{
		showContours = std::strcmp(argv[1], "true") == 0;
	}
	float threshold = 0.01f;
	if(argc >= 3)
	{
		threshold = std::stof(argv[2]);
	}
	float viewWindowTopLeftCornerX = -10.0f;
	float viewWindowTopLeftCornerY = 10.0f;
	float viewWindowBottomRightCornerX = 10.0f;
	float viewWindowBottomRightCornerY = -10.0f;
	if(argc == 4)
	{
		viewWindowTopLeftCornerX = -1.0f * std::stof(argv[3]);
		viewWindowTopLeftCornerY = viewWindowTopLeftCornerX;
		viewWindowBottomRightCornerX = viewWindowTopLeftCornerX;
		viewWindowBottomRightCornerY = -1.0f * viewWindowTopLeftCornerX;
	}
	if(argc >= 7)
	{
		viewWindowTopLeftCornerX = std::stof(argv[3]);
		viewWindowTopLeftCornerY = std::stof(argv[4]);
		viewWindowBottomRightCornerX = std::stof(argv[5]);
		viewWindowBottomRightCornerY = std::stof(argv[6]);
	}
	int width = 1024;
	int height = 1024;
	if(argc == 8)
	{
		width = std::stoi(argv[7]);
		height = width;
	}
	else if(argc >= 9)
	{
		width = std::stoi(argv[7]);
		height = std::stoi(argv[8]);
	}
	int subdivisionResolution = 1;
	float subdivisionThreshold = threshold;
	if(argc >= 11)
	{
		subdivisionResolution = std::stoi(argv[9]);
		subdivisionThreshold = std::stof(argv[10]);
	}
	
	unsigned char* imageData = new unsigned char[width * height];
	
	unsigned int numThreads = std::thread::hardware_concurrency();
	if(numThreads == 0) { numThreads = 1; }
	std::thread* threads = new std::thread[numThreads];
	std::cout << "using " << numThreads << " threads" << std::endl;
	{
		size_t numRowsToProcessPerThread = static_cast<size_t>(height) / numThreads;
		size_t i;
		for(i = 0; i < numThreads-1; i++)
		{
			if(showContours)
			{
				threads[i] = std::thread(threadFunctionContour, imageData, numRowsToProcessPerThread * i, numRowsToProcessPerThread * (i+1), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY);
			}
			else
			{
				threads[i] = std::thread(threadFunction, imageData, numRowsToProcessPerThread * i, numRowsToProcessPerThread * (i+1), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY, subdivisionResolution, subdivisionThreshold);
			}
		}
		if(showContours)
		{
			threads[i] = std::thread(threadFunctionContour, imageData, numRowsToProcessPerThread * i, static_cast<size_t>(height), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY);
		}
		else
		{
			threads[i] = std::thread(threadFunction, imageData, numRowsToProcessPerThread * i, static_cast<size_t>(height), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY, subdivisionResolution, subdivisionThreshold);
		}
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
