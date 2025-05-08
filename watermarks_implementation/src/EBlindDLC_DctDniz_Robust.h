#ifndef EBlindDLC_DCTDNIZ_ROBUST
#define EBlindDLC_DCTDNIZ_ROBUST

#include "Watermark.h"

class EBlindDLC_DctDniz_Robust : public Watermark
{
public:
	EBlindDLC_DctDniz_Robust(double alpha = -1, double detectThreshold = -1, std::weak_ptr<VideoFrame> preference = std::weak_ptr<VideoFrame>());
	void setAlpha(double alpha);
	void setDetectThreshold(double detectThreshold);
	void setReference(std::weak_ptr<VideoFrame> preference);

	bool embed(std::shared_ptr<VideoFrame> pframe, bool value);
	DetectResult detect(const std::shared_ptr<VideoFrame> pframe);

	static std::shared_ptr<VideoFrame> createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue, float robustThreshold);

private:
	double                       m_alpha;
	double                       m_detectThreshold;
	std::weak_ptr<VideoFrame>    m_preference;

};

#endif
