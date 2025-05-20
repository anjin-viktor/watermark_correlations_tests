#ifndef E_BLIND_DLC
#define E_BLIND_DLC

#include "Watermark.h"
#include "Detector.h"

class EBlindDLC : public Watermark
{
public:
	EBlindDLC(double alpha = -1, double detectThreshold = -1, std::weak_ptr<VideoFrame> preference = std::weak_ptr<VideoFrame>(), Detector::Type detectorType = Detector::LINEAR);
	virtual ~EBlindDLC();
	void setAlpha(double alpha);
	void setDetectThreshold(double detectThreshold);
	void setReference(std::weak_ptr<VideoFrame> preference);

	bool embed(std::shared_ptr<VideoFrame> pframe, bool value);
	DetectResult detect(const std::shared_ptr<VideoFrame> pframe);
	bool getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation);

	static std::shared_ptr<VideoFrame> createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue);

private:
	DetectResult detectInternal(const std::shared_ptr<VideoFrame> pframe, double *pcorrelation);

	double                       m_alpha;
	double                       m_detectThreshold;
	std::weak_ptr<VideoFrame>    m_preference;
	Detector::Type               m_detectorType;

};

#endif
