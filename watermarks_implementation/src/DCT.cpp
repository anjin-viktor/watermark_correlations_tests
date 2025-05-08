#include "DCT.h"

#include <opencv2/opencv.hpp>

std::vector<float> DCT(std::vector<uint8_t> block, std::size_t blockSize)
{
	std::vector<float> srcData(block.size());
	std::vector<float> dstData(block.size());
	for (int i = 0; i < block.size(); i++)
		srcData[i] = block[i];

	cv::Mat src = cv::Mat((int)blockSize, (int)blockSize, CV_32F, &srcData[0]);
	cv::Mat dst = cv::Mat((int)blockSize, (int)blockSize, CV_32F, &dstData[0]);

	cv::dct(src, dst);

	return dstData;
}

std::vector<uint8_t> iDCT(std::vector<float> block, std::size_t blockSize)
{
	std::vector<float> srcData(block.size());
	std::vector<float> dstData(block.size());
	for (int i = 0; i < block.size(); i++)
		srcData[i] = block[i];

	cv::Mat src = cv::Mat((int)blockSize, (int)blockSize, CV_32F, &srcData[0]);
	cv::Mat dst = cv::Mat((int)blockSize, (int)blockSize, CV_32F, &dstData[0]);

	cv::idct(src, dst);
	std::vector<uint8_t> res(blockSize * blockSize);
	for (std::size_t i = 0; i < dstData.size(); i++)
	{
		int val = (int)dstData[i];
		if (val < 0)
			val = 0;
		else if (val > 255)
			val = 255;

		res[i] = val;
	}

	return res;
}
