#include "pch.h"
#include <array>
#include <immintrin.h>
#include <algorithm>
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

using PS_8 = __m256;
using Pi32_8 = __m256i;
using Mask_8 = __mmask8;

float maxF(float a, float b)
{
	return (a * (a>b)) + (b * (a<=b));
}
float absF(float n)
{
	return (n * (n>0.0f)) + (-n * (n<=0.0f));
}

float func(float x, float y)
{
	return absF( (y-std::sin(x)) * (maxF(absF(x+y)+absF(x-y)-1.0f,0.0f) + maxF(0.25f-(x*x)-(y*y),0.0f)) );
}





template<float val>
constexpr void constSetPS(PS_8& reg)
{
	*reinterpret_cast<std::array<float, 8>*>(&reg) = {val, val, val, val, val, val, val, val};
}
template<int32_t val>
constexpr void constSetPi32(Pi32_8& reg)
{
	*reinterpret_cast<std::array<int32_t, 8>*>(&reg) = {val, val, val, val, val, val, val, val};
}
const PS_8 FPABS_SIGN_BIT_MASK = _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000));





void threadFunction(unsigned char* imageData, size_t startPixleY, size_t endPixleY, float threshold, int width, int height, float viewWindowTopLeftCornerX, float viewWindowTopLeftCornerY, float viewWindowBottomRightCornerX, float viewWindowBottomRightCornerY, int subdivisionResolution, float subdivisionThreshold)
{
	float startX = viewWindowTopLeftCornerX;
	float startY = viewWindowTopLeftCornerY - ((viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) * (static_cast<float>(startPixleY) / static_cast<float>(width)));

	float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);

	//float x = startX;
	//float y = startY;
	
	//generate a vector of x values starting at startX and incrementing by xIncrement
	//r3 = (xIncrement * r1) + startX
	PS_8 r1;
	*reinterpret_cast<std::array<float, 8>*>(&r1) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
	PS_8 r2 = _mm256_set1_ps(xIncrement);
	PS_8 r3 = _mm256_set1_ps(startX);
	r3 = _mm256_fmadd_ps(r2, r1, r3);

	//generate a vector to increase r3 by each iteration
	//r2 = xIncrement * 8;
	*reinterpret_cast<std::array<float, 8>*>(&r1) = {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f};
	r2 = _mm256_set1_ps(xIncrement);
	r2 = _mm256_mul_ps(r2, r1);

	//generate a vector of the y values. They will stay constant
	//r1 is a tempuary value for the first two steps and is no longer needed so can be reused.
	r1 = _mm256_set1_ps(startY);
	PS_8 r4 = _mm256_set1_ps(yIncrement);
	
	//r1 = y
	//r2 = xIncrement adjusted for SIMD
	//r3 = x
	//r4 = yIncrement
	
	size_t endPixleX = static_cast<size_t>(width);
	for(size_t i = startPixleY; i < endPixleY; i++)
	{
		for(size_t j = 0; j < endPixleX; j+=8)
		{
			#include "function.cpp"
			
			float* val = reinterpret_cast<float*>(&r5);
			for(size_t k = 0; k < 8; k++)
			{
				/*
				if(val[k] < threshold) [[unlikely]]
				{
					std::cout << ((i * width) + j + k) << std::endl;
				}
				*/
				std::cout << "y=" << i << ", x=" << (j+k) << " " << val[k] << "\n";
			}
			
			r6 = _mm256_set1_ps(threshold);
			r5 = _mm256_cmp_ps(r5, r6, _CMP_GT_OQ);
			
			_mm256_storeu_ps(reinterpret_cast<float*>(imageData) + (i * width) + j, r5);
			
			r3 = _mm256_add_ps(r3, r2); //x += xIncrement
		}
		r1 = _mm256_add_ps(r1, r4); // y += yIncrement
	}
}
void threadFunctionContour(unsigned char* imageData, size_t startPixleY, size_t endPixleY, float threshold, int width, int height, float viewWindowTopLeftCornerX, float viewWindowTopLeftCornerY, float viewWindowBottomRightCornerX, float viewWindowBottomRightCornerY)
{
	float startX = viewWindowTopLeftCornerX;
	float startY = viewWindowTopLeftCornerY - ((viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) * (static_cast<float>(startPixleY) / static_cast<float>(width)));
	
	float xIncrement = (viewWindowBottomRightCornerX - viewWindowTopLeftCornerX) / static_cast<float>(width);
	float yIncrement = (viewWindowTopLeftCornerY - viewWindowBottomRightCornerY) / static_cast<float>(height);
	
	float x = startX;
	float y = startY;
	for(size_t i = startPixleY * static_cast<size_t>(width); i < endPixleY * static_cast<size_t>(width); i++)
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
		//x -= cumulativeXIncrement * (i % static_cast<size_t>(width) == 0);
		if(i % static_cast<size_t>(width) == 0)
		{
			x = startX;
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
	
	//unsigned char* imageData = new unsigned char[width * height];
	unsigned char* imageData = reinterpret_cast<unsigned char*>(std::aligned_alloc(64, ((((width * height) / 64) * 64) + 64) * sizeof(unsigned char) * 4));
	
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
				//threads[i] = std::thread(threadFunctionContour, imageData, numRowsToProcessPerThread * i, numRowsToProcessPerThread * (i+1), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY);
			}
			else
			{
				threads[i] = std::thread(threadFunction, imageData, numRowsToProcessPerThread * i, numRowsToProcessPerThread * (i+1), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY, subdivisionResolution, subdivisionThreshold);
				threads[i].join();
			}
		}
		if(showContours)
		{
			//threads[i] = std::thread(threadFunctionContour, imageData, numRowsToProcessPerThread * i, static_cast<size_t>(height), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY);
			std::cout << "contours not supported" << std::endl;
		}
		else
		{
			threads[i] = std::thread(threadFunction, imageData, numRowsToProcessPerThread * i, static_cast<size_t>(height), threshold, width, height, viewWindowTopLeftCornerX, viewWindowTopLeftCornerY, viewWindowBottomRightCornerX, viewWindowBottomRightCornerY, subdivisionResolution, subdivisionThreshold);
			threads[i].join();
		}
		for(size_t j = 0; j < numThreads; j++)
		{
			//threads[j].join();
		}
	}
	delete[] threads;
	
	TimeType endTime = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();
	auto t2 = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
	std::cout << "Time taken: " << ((t1 - t2) * 0.001f) << "ms" << std::endl;
	
	stbi_write_png("image.png", width, height, 4, imageData, static_cast<int>(sizeof(char))*width);
	
	//delete[] imageData;
	std::free(imageData);
	
	return 0;
}
