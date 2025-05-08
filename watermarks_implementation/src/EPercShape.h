#ifndef E_PERC_SHAPE
#define E_PERC_SHAPE

#include "EBlindDLC.h"

//Christine I. Podilchuk and Wenjun Zeng. Image-adaptive watermarking using visual models. IEEE Journal of Selected Areas in Communication, 16(4):525–539, 1998.

class EPercShape : public Watermark
{
public:
	EPercShape(double detectThreshold = -1, std::weak_ptr<VideoFrame> porigin = std::weak_ptr<VideoFrame>(), std::weak_ptr<VideoFrame> preference = std::weak_ptr<VideoFrame>());

	bool embed(std::shared_ptr<VideoFrame> pframe, bool value);
	DetectResult detect(const std::shared_ptr<VideoFrame> pframe);
	bool getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation);


	static std::shared_ptr<VideoFrame> createReference(const std::shared_ptr<VideoFrame> pframe, double strength); //strength from 0 to 1

private:
	std::weak_ptr<VideoFrame>     m_preference;
	std::weak_ptr<VideoFrame>     m_porig;
	double                        m_detectThreshold;
};

#endif
