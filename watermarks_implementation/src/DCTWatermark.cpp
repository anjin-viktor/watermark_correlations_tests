#include "DCTWatermark.h"

#include <fstream>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "VideoFrameBlockProcessor.h"
#include "Detector.h"

#include <random>

//Version with adding abs value of DCT coefficient during embedding:  Barni M.et al.A DCT - domain system for robust image watermarking //Signal processing. – 1998. – Ò. 66. – ¹. 3. – Ñ. 357-372.
//Version without adding abs value of DCT coefficient: Cox I. J. et al. Secure spread spectrum watermarking for multimedia //IEEE transactions on image processing. – 1997. – Ò. 6. – ¹. 12. – Ñ. 1673-1687.
//Middle frequcny watermarking: Hsu C.T., Wu J.L.DCT - based watermarking for video //IEEE Transactions on Consumer Electronics. – 1998. – Ò. 44. – ¹. 1. – Ñ. 206-216.

namespace 
{
	const int IZigzagIndex[] =
	{ 0,1,5,6,14,15,27,28,
		2,4,7,13,16,26,29,42,
		3,8,12,17,25,30,41,43,
		9,11,18,24,31,40,44,53,
		10,19,23,32,39,45,52,54,
		20,22,33,38,46,51,55,60,
		21,34,37,47,50,56,59,61,
		35,36,48,49,57,58,62,63 };

	const int ZigzagIndex[] =
	{ 0,1,8,16,9,2,3,10,
		17,24,32,25,18,11,4,5,
		12,19,26,33,40,48,41,34,
		27,20,13,6,7,14,21,28,
		35,42,49,56,57,50,43,36,
		29,22,15,23,30,37,44,51,
		58,59,52,45,38,31,39,46,
		53,60,61,54,47,55,62,63 };


	std::vector<float> zigzagScan(std::vector<float> data)
	{
		std::vector<float> result(data);

		for (int i = 0; i < 64; i++)
		{
			int idx = ZigzagIndex[i];
			result[i] = data[idx];
		}
		return result;
	}

	bool isZigzagFrequency(std::size_t pos)
	{
		bool isMiddleFrequence = pos >= 12 && pos <= 25 || pos >= 34 && pos <= 35;

		return isMiddleFrequence;
	}

	std::vector<float> izigzagScan(std::vector<float> data)
	{
		std::vector<float> result(data);

		for (int i = 0; i < 64; i++)
		{
			int idx = IZigzagIndex[i];
			result[i] = data[idx];
		}
		return result;
	}
}

DCTWatermark::DCTWatermark(double alpha, double detectThreshold, int skipCount, std::vector<int> reference, WatermarkType type, Detector::Type detectorType, DCTWatermark::EmbeddingType embeddingType) : Watermark(),
	m_alpha(alpha),
	m_detectThreshold(detectThreshold),
	m_skipCount(skipCount),
	m_reference(reference),
	m_watermarkType(type),
	m_detectorType(detectorType),
	m_embeddingType(embeddingType)
{
}

void DCTWatermark::setAlpha(double alpha)
{
	m_alpha = alpha;
}

void DCTWatermark::setDetectThreshold(double detectThreshold)
{
	m_detectThreshold = detectThreshold;
}

void DCTWatermark::setReference(const std::vector<int> &reference)
{
	m_reference = reference;
}

void DCTWatermark::setSkipCount(int skipCount)
{
	m_skipCount = skipCount;
}

bool DCTWatermark::embed(std::shared_ptr<VideoFrame> pframe, bool value)
{
	if (m_reference.empty())
		return false;
	if (m_alpha < 0)
		return false;

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);

	std::size_t expectedReferenceSize = blockProcessor.getBlockCount() * (m_embeddingType == DCTWatermark::MIDDLE_FREQUENCY ? 16 : (BLOCK_SIZE * BLOCK_SIZE - m_skipCount));
	if (expectedReferenceSize != m_reference.size())
		return false;

	std::size_t blockCount = blockProcessor.getBlockCount();
	std::size_t referenceIdx = 0;
	for (std::size_t idx = 0; idx < blockCount; idx++)
	{
		std::vector<uint8_t> blockData = blockProcessor.getBlock(idx);

		std::vector<float> srcData(blockData.size());
		std::vector<float> dstData(blockData.size());
		for (int i = 0; i < blockData.size(); i++)
			srcData[i] = blockData[i];

		cv::Mat src = cv::Mat((int)BLOCK_SIZE, (int)BLOCK_SIZE, CV_32F, &srcData[0]);
		cv::Mat dst = cv::Mat((int)BLOCK_SIZE, (int)BLOCK_SIZE, CV_32F, &dstData[0]);

		cv::dct(src, dst);
		std::vector<float> zigzag = zigzagScan(dstData);

		for (std::size_t pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++)
		{
			bool needEmbedding = false;
			if (m_embeddingType == DCTWatermark::MIDDLE_FREQUENCY)
				needEmbedding = isZigzagFrequency(pos);
			else if (m_embeddingType == DCTWatermark::SKIP_FIRST)
				needEmbedding = pos >= m_skipCount;

			if (needEmbedding)
			{
				double val = zigzag[pos];
				double wr = m_reference[referenceIdx] * m_alpha;
				if (m_watermarkType == DCTWatermark::BARNI)
				{
					wr *= std::abs(val);
				}

				if (value)
					val += wr;
				else
					val -= wr;
				zigzag[pos] = (float)val;

				referenceIdx++;
			}
		}

		dstData = izigzagScan(zigzag);
		dst = cv::Mat((int)BLOCK_SIZE, (int)BLOCK_SIZE, CV_32F, &dstData[0]);

		cv::idct(dst, src);
		for (std::size_t i = 0; i < blockData.size(); i++)
		{
			float val = srcData[i];
			if (val < 0)
				val = 0;
			else if (val > 255)
				val = 255;

			blockData[i] = (uint8_t)val;
		}

		blockProcessor.setBlock(blockData, idx);
	}

	return true;
}

Watermark::DetectResult DCTWatermark::detect(const std::shared_ptr<VideoFrame> pframe)
{
	return detectInternal(pframe, NULL, NULL);
}

Watermark::DetectResult DCTWatermark::detectInternal(const std::shared_ptr<VideoFrame> pframe, double* pcorrelation, double* averageDCT)
{
	if (m_reference.empty())
		return Watermark::DetectResult::FAILED;
	if (m_detectThreshold < 0)
		return Watermark::DetectResult::FAILED;

	VideoFrameBlockProcessor blockProcessor(pframe, BLOCK_SIZE);

	std::size_t expectedReferenceSize = blockProcessor.getBlockCount() * (m_embeddingType == DCTWatermark::MIDDLE_FREQUENCY ? 16 : (BLOCK_SIZE * BLOCK_SIZE - m_skipCount));
	if (expectedReferenceSize != m_reference.size())
		return Watermark::DetectResult::FAILED;

	std::vector<int> extractedData(expectedReferenceSize);
	std::size_t blockCount = blockProcessor.getBlockCount();
	std::size_t extractIdx = 0;
	double sumDCT = 0;
	for (std::size_t idx = 0; idx < blockCount; idx++)
	{
		std::vector<uint8_t> blockData = blockProcessor.getBlock(idx);

		std::vector<float> srcData(blockData.size());
		std::vector<float> dstData(blockData.size());
		for (int i = 0; i < blockData.size(); i++)
			srcData[i] = blockData[i];

		cv::Mat src = cv::Mat((int)BLOCK_SIZE, (int)BLOCK_SIZE, CV_32F, &srcData[0]);
		cv::Mat dst = cv::Mat((int)BLOCK_SIZE, (int)BLOCK_SIZE, CV_32F, &dstData[0]);

		cv::dct(src, dst);
		std::vector<float> zigzag = zigzagScan(dstData);

		for (std::size_t pos = 0; pos < BLOCK_SIZE * BLOCK_SIZE; pos++)
		{
			bool needExtracting = m_embeddingType == DCTWatermark::SKIP_FIRST;
			if (m_embeddingType == DCTWatermark::MIDDLE_FREQUENCY)
				needExtracting = isZigzagFrequency(pos);
			else if (m_embeddingType == DCTWatermark::SKIP_FIRST)
				needExtracting = pos >= m_skipCount;

			if (needExtracting)
			{
				float val = zigzag[pos];
				extractedData[extractIdx] = (int)val;
				sumDCT += std::fabs(val);
				extractIdx++;
			}
		}
	}

	if (averageDCT)
		*averageDCT = sumDCT / extractedData.size();

	Detector::Result res = Detector::FAILED;
	if (m_detectorType == Detector::CORRELATION_COEFFICIENT)
	{
		if(m_watermarkType == DCTWatermark::LEGACY_CC)
			res = Detector::CorrelationCoefficientLegacy(extractedData, m_reference, m_detectThreshold, pcorrelation);
		else
			res = Detector::CorrelationCoefficient(extractedData, m_reference, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::LINEAR)
	{
		res = Detector::LinearCorrelation(extractedData, m_reference, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::NORMALIZED)
	{
		res = Detector::NormalizedCorrelation(extractedData, m_reference, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::PEARSON_COEFFICIENT)
	{
		res = Detector::PearsonCorrelation(extractedData, m_reference, m_detectThreshold, pcorrelation);
	}

	return (Watermark::DetectResult) res;
}

bool DCTWatermark::getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation)
{
	if (m_reference.empty())
		return false;

	detectInternal(pframe, &correlation, NULL);
	return true;
}

bool DCTWatermark::getAverageDCT(const std::shared_ptr<VideoFrame> pframe, double& averageDCT)
{
	if (m_reference.empty())
		return false;

	detectInternal(pframe, NULL, &averageDCT);
	return true;
}


std::vector<int> DCTWatermark::createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue, int skipCount, bool BarniVersion, DCTWatermark::EmbeddingType embeddingType)
{
	if (skipCount >= BLOCK_SIZE * BLOCK_SIZE)
		return std::vector<int>();

	std::size_t widthInBlocks = pframe->width() / BLOCK_SIZE;
	std::size_t heightInBlocks = pframe->height() / BLOCK_SIZE;

	std::size_t blocksCount = widthInBlocks * heightInBlocks;
	std::size_t referenceSize = blocksCount * (embeddingType == DCTWatermark::MIDDLE_FREQUENCY ? 16 : (BLOCK_SIZE * BLOCK_SIZE - skipCount));
	if (pframe->colorFormat() == VideoFrame::Color)
		referenceSize *= 3;

	std::vector<int> reference(referenceSize);

	auto genRandom = [&maxValue]() {
		return rand() % maxValue;
	};

	std::generate(reference.begin(), reference.end(), genRandom);

	if (BarniVersion)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<double> dist(0.0, 1.0);  // mean=0, stddev=1

		for (std::size_t i = 0; i < reference.size(); i++)
		{
			int val = dist(gen) * 255;
			reference[i] = val;
			if (reference[i] < 0)
				reference[i] *= -1;
		}
	}

	return reference;
}

std::vector<int> DCTWatermark::cropReference(const std::vector<int>& reference, int skipCount, std::size_t x, std::size_t y, std::size_t width, std::size_t height, std::size_t widthOrig, std::size_t heightOrig)
{
	if (x % BLOCK_SIZE || y % BLOCK_SIZE)
		return std::vector<int>();

	std::size_t blockSize = BLOCK_SIZE * BLOCK_SIZE - skipCount;
	std::size_t blockCnt = reference.size() / blockSize;

	std::size_t xStart = x / BLOCK_SIZE;
	std::size_t xEnd = xStart + width / BLOCK_SIZE;
	std::size_t yStart= y / BLOCK_SIZE;
	std::size_t yEnd = yStart + height / BLOCK_SIZE;

	std::size_t widthBlocksOrig = widthOrig / BLOCK_SIZE;
	std::size_t heightBlocksOrig = heightOrig / BLOCK_SIZE;
	std::size_t widthBlocks = width / BLOCK_SIZE;

	std::size_t planeBlocksCnt = widthBlocksOrig * heightBlocksOrig;
	std::size_t planeCnt = blockCnt / planeBlocksCnt;
	std::size_t planeCroppBlocksCnt = (xEnd - xStart) * (yEnd - yStart);

	std::size_t resSize = planeCnt * (xEnd - xStart) * (yEnd - yStart) * blockSize;
	std::vector<int> res(resSize);
	for (std::size_t y = yStart; y < yEnd; y++)
	{
		for (std::size_t x = xStart; x < xEnd; x++)
		{
			for (std::size_t i = 0; i < planeCnt; i++)
			{
				std::size_t offsetSrc = (i + (y * widthBlocksOrig + x) * planeCnt) * blockSize;
				std::size_t offsetDst = (i + ((y - yStart) * widthBlocks + x - xStart) * planeCnt) * blockSize;

				std::copy(reference.begin() + offsetSrc, reference.begin() + offsetSrc + blockSize, res.begin() + offsetDst);
			}
		}
	}

	return res;
}

void DCTWatermark::referenceStore(const std::vector<int>& data, const std::string& filename)
{
	std::ofstream file(filename);
	for (size_t i = 0; i < data.size(); i++)
	{
		file << data[i];
		if (i + 1 != data.size())
			file << ' ';
	}
}

std::vector<int> DCTWatermark::referenceLoad(const std::string& filename)
{
	std::ifstream file(filename);
	std::vector<int> loadedData;

	while (!file.eof())
	{
		int value;
		file >> value;
		loadedData.push_back(value);
	}
	return loadedData;
}
