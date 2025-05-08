#ifndef VIDEO_FRAME_BLOCK_PROCESSOR
#define VIDEO_FRAME_BLOCK_PROCESSOR

#include <memory>

#include "VideoFrame.h"

class VideoFrameBlockProcessor
{
public:
	VideoFrameBlockProcessor(std::shared_ptr<VideoFrame> pframe, std::size_t blockSize);

	std::size_t getBlockCount() const;
	std::vector<uint8_t> getBlock(std::size_t idx) const;
	bool setBlock(const std::vector<uint8_t> blockData, std::size_t idx);
private:
	mutable std::weak_ptr<VideoFrame>     m_pframe;
	std::size_t                           m_blockSize;
};


#endif
