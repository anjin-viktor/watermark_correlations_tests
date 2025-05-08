#ifndef WATERMARK_H_
#define WATERMARK_H_

#include "VideoFrame.h"

class Watermark
{
public:
	enum DetectResult
	{
		FAILED,
		TRUE,
		FALSE,
		NO_WATERMARK
	};

	virtual ~Watermark() {};
	virtual bool embed(std::shared_ptr<VideoFrame> pframe, bool value) = 0;
	virtual DetectResult detect(const std::shared_ptr<VideoFrame> pframe) = 0;
	virtual bool getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation) { return false;  } //unsupported by default

};

#endif
