#include "EPercShape.h"
#include "WatermarkReference.h"
#include "Detector.h"
#include "VisualMasking.h"
#include "VideoFrameBlockProcessor.h"
#include "DCT.h"


EPercShape::EPercShape(double detectThreshold, std::weak_ptr<VideoFrame> porigin, std::weak_ptr<VideoFrame> preference):
	m_detectThreshold(detectThreshold),
	m_preference(preference),
	m_porig(porigin)
{
}

std::shared_ptr<VideoFrame> EPercShape::createReference(const std::shared_ptr<VideoFrame> pframe, double strength)
{
	VisualMasking masking(pframe);
	std::vector<VisualMasking::Block> sBlocks = masking.getSBlocks();

	std::shared_ptr<VideoFrame> pFrameWatermarked = std::make_shared<VideoFrame>(*pframe);

	VideoFrameBlockProcessor blockProcessor(pFrameWatermarked, VisualMasking::BLOCK_SIZE);
	std::size_t blockCount = blockProcessor.getBlockCount();

	if (blockCount != sBlocks.size())
		return pFrameWatermarked;

	for (std::size_t i = 0; i < blockCount; i++)
	{
		std::vector<uint8_t> block = blockProcessor.getBlock(i);
		std::vector<float> blockDCT = DCT(block, VisualMasking::BLOCK_SIZE);

		for (std::size_t j = 0; j < VisualMasking::BLOCK_SIZE * VisualMasking::BLOCK_SIZE; j++)
		{
			if (blockDCT[j] > sBlocks[i][j])
			{
				float r = (float) (rand()) / float (RAND_MAX / strength);
				blockDCT[j] += sBlocks[i][j] * r;
			}
		}

		block = iDCT(blockDCT, VisualMasking::BLOCK_SIZE);
		blockProcessor.setBlock(block, i);
	}

	return pFrameWatermarked->sub(pframe);
}

bool EPercShape::embed(std::shared_ptr<VideoFrame> pframe, bool value)
{
	if (!value)
		return false;
	std::shared_ptr<VideoFrame> preference = m_preference.lock();
	if (!preference)
		return false;

	if (!pframe)
		return false;

	*pframe = *pframe->add(preference);

	return true;
}

Watermark::DetectResult EPercShape::detect(const std::shared_ptr<VideoFrame> pframe)
{
	std::shared_ptr<VideoFrame> preference = m_preference.lock();
	if (!preference)
		return Watermark::DetectResult::NO_WATERMARK;

	std::shared_ptr<VideoFrame> porig = m_porig.lock();
	if (!porig)
		return Watermark::DetectResult::NO_WATERMARK;


	std::shared_ptr<VideoFrame> pdiff = pframe->sub(porig);

	Detector::Result res = Detector::Result::FAILED;

	if (pframe->colorFormat() == VideoFrame::Color)
		res = Detector::NormalizedCorrelationRGB(pdiff, preference, m_detectThreshold);
	else
		res = Detector::CorrelationCoefficientGrayscale(pdiff, preference, m_detectThreshold);

	return (EBlindDLC::DetectResult)res;

}

bool EPercShape::getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation)
{
	std::shared_ptr<VideoFrame> preference = m_preference.lock();
	std::shared_ptr<VideoFrame> porig = m_porig.lock();
	if (!preference || !porig)
		return false;

	std::shared_ptr<VideoFrame> pdiff = pframe->sub(porig);

	if (pframe->colorFormat() == VideoFrame::Color)
		Detector::NormalizedCorrelationRGB(pdiff, preference, m_detectThreshold, &correlation);
	else
		Detector::CorrelationCoefficientGrayscale(pdiff, preference, m_detectThreshold, &correlation);

	return true;

}
