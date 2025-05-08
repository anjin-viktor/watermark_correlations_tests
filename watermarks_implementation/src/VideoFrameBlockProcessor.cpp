#include "VideoFrameBlockProcessor.h"

VideoFrameBlockProcessor::VideoFrameBlockProcessor(std::shared_ptr<VideoFrame> pframe, std::size_t blockSize):
	m_pframe(pframe),
	m_blockSize(blockSize)
{
}

std::size_t VideoFrameBlockProcessor::getBlockCount() const
{
	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return 0;

	std::size_t widthInBlocks = pframe->width() / m_blockSize;
	std::size_t heightInBlocks = pframe->height() / m_blockSize;

	std::size_t blocksCount = widthInBlocks * heightInBlocks;

	if (pframe->colorFormat() == VideoFrame::Color)
		blocksCount *= 3;

	return blocksCount;
}


std::vector<uint8_t> VideoFrameBlockProcessor::getBlock(std::size_t idx) const
{
	if (idx >= getBlockCount())
		return std::vector<uint8_t>();

	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return std::vector<uint8_t>();

	std::size_t widthInBlocks = pframe->width() / m_blockSize;
	std::size_t heightInBlocks = pframe->height() / m_blockSize;

	std::size_t blockIndex = idx;
	std::size_t offset = 0;
	std::size_t pixelSize = 1;
	if (pframe->colorFormat() == VideoFrame::Color)
	{
		blockIndex /= 3;
		offset = idx % 3;
		pixelSize = 3;
	}

	std::size_t posBlockX = blockIndex % widthInBlocks;
	std::size_t posBlockY = blockIndex / widthInBlocks;

	std::size_t posXStart = posBlockX * m_blockSize;
	std::size_t posYStart = posBlockY * m_blockSize;

	std::vector<uint8_t> result(m_blockSize * m_blockSize);
	uint8_t* pdata = pframe->data(0);
	std::size_t stride = pframe->stride(0);

	for (std::size_t posX = 0; posX < m_blockSize; posX++)
	{
		for (std::size_t posY = 0; posY < m_blockSize; posY++)
		{
			result[posY * m_blockSize + posX] = pdata[offset + (posXStart + posX) * pixelSize + (posYStart + posY) * stride];
		}
	}

	return result;
}

bool VideoFrameBlockProcessor::setBlock(const std::vector<uint8_t> blockData, std::size_t idx)
{
	if (idx >= getBlockCount())
		return false;

	std::shared_ptr<VideoFrame> pframe = m_pframe.lock();
	if (!pframe)
		return false;

	std::size_t widthInBlocks = pframe->width() / m_blockSize;
	std::size_t heightInBlocks = pframe->height() / m_blockSize;

	std::size_t blockIndex = idx;
	std::size_t offset = 0;
	std::size_t pixelSize = 1;
	if (pframe->colorFormat() == VideoFrame::Color)
	{
		blockIndex /= 3;
		offset = idx % 3;
		pixelSize = 3;
	}

	std::size_t posBlockX = blockIndex % widthInBlocks;
	std::size_t posBlockY = blockIndex / widthInBlocks;

	std::size_t posXStart = posBlockX * m_blockSize;
	std::size_t posYStart = posBlockY * m_blockSize;

	std::vector<uint8_t> result(m_blockSize * m_blockSize);
	uint8_t* pdata = pframe->data(0);
	std::size_t stride = pframe->stride(0);

	for (std::size_t posX = 0; posX < m_blockSize; posX++)
	{
		for (std::size_t posY = 0; posY < m_blockSize; posY++)
		{
			pdata[offset + (posXStart + posX) * pixelSize + (posYStart + posY) * stride] = blockData[posY * m_blockSize + posX];
		}
	}

	return true;
}
