#include "VisualMasking.h"

#include <numeric> 
#include <cmath>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "VideoFrameBlockProcessor.h"
#include "DCT.h"

namespace 
{
	const static double DCTFrequencySensitivity[64] = {
		1.40, 1.01, 1.16, 1.66, 2.40, 3.43, 4.79, 6.56,
		1.01, 1.45, 1.32, 1.52, 2.00, 2.71, 3.67, 4.93,
		1.16, 1.32, 2.24, 2.59, 2.98, 3.64, 4.60, 5.88,
		1.66, 1.52, 2.59, 3.77, 4.55, 5.30, 6.28, 7.60,
		2.40, 2.00, 2.98, 4.55, 6.15, 7.46, 8.71, 10.17,
		3.43, 2.71, 3.64, 5.30, 7.46, 9.62, 11.58, 13.51,
		4.79, 3.67, 4.60, 6.28, 8.71, 11.58, 14.50, 17.29,
		6.56, 4.93, 5.88, 7.60, 10.17, 13.51, 17.29, 21.15,
	};
}

VisualMasking::VisualMasking(std::shared_ptr<VideoFrame> pframe):
	m_pframe(pframe)
{
	calcDCCoefficients();

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);
	std::size_t blockCount = blockProcessor.getBlockCount();
	m_SBlocks.resize(blockCount);

	for (std::size_t i = 0; i < blockCount; i++)
		m_SBlocks[i] = contrastMask(i);
}

void VisualMasking::calcDCCoefficients()
{
	if (!m_DCCoefficients.empty())
		return;

	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return;

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);
	std::size_t blockCount = blockProcessor.getBlockCount();
	m_DCCoefficients.resize(blockCount);

	for (std::size_t idx = 0; idx < blockCount; idx++)
	{
		std::vector<uint8_t> blockData = blockProcessor.getBlock(idx);

		std::vector<float> srcData(blockData.size());
		std::vector<float> dstData(blockData.size());
		for (int i = 0; i < blockData.size(); i++)
			srcData[i] = blockData[i];

		cv::Mat src = cv::Mat(BLOCK_SIZE, BLOCK_SIZE, CV_32F, &srcData[0]);
		cv::Mat dst = cv::Mat(BLOCK_SIZE, BLOCK_SIZE, CV_32F, &dstData[0]);

		cv::dct(src, dst);
		m_DCCoefficients[idx] = dstData[0];

		double sum = std::accumulate(m_DCCoefficients.begin(), m_DCCoefficients.end(), 0.0);
		m_DCAvg = sum / blockCount;
	}
}

std::vector<double> VisualMasking::luminanceMask(std::size_t k)
{
	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return std::vector<double>();
	if(k >= m_DCCoefficients.size())
		return std::vector<double>();

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);

	double DC = m_DCCoefficients[k];

	double cof = DC / m_DCAvg;
	double cofPow = std::pow(cof, 0.649);

	std::vector<double> res(BLOCK_SIZE * BLOCK_SIZE);
	for (std::size_t i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++)
	{
		res[i] = (double)DCTFrequencySensitivity[i] * cofPow;
	}

	return res;
}

std::vector<double> VisualMasking::contrastMask(std::size_t k)
{
	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return std::vector<double>();
	if (k >= m_DCCoefficients.size())
		return std::vector<double>();

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);
	std::vector<uint8_t> block = blockProcessor.getBlock(k);
	std::vector<float> c = DCT(block, 8);
	std::vector<double> t = luminanceMask(k);

	std::vector<double> s(BLOCK_SIZE * BLOCK_SIZE);

	static const double w = 0.7;
	for (std::size_t i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++)
	{
		double tPow = std::pow(t[i], 1 - w);
		double C = std::fabs((float)c[i]);
		double cPow = std::pow(C, w);
		s[i] = (double)std::max((double)t[i], cPow * tPow);
	}

	return s;
}

std::vector<VisualMasking::Block> VisualMasking::getSBlocks()
{
	return m_SBlocks;
}