#include "EBlindDLC_DctDniz_Robust.h"
#include "WatermarkReference.h"
#include "Detector.h"

EBlindDLC_DctDniz_Robust::EBlindDLC_DctDniz_Robust(double alpha, double detectThreshold, std::weak_ptr<VideoFrame> preference): Watermark(),
	m_alpha(alpha),
	m_detectThreshold(detectThreshold),
	m_preference(preference)
{
}

void EBlindDLC_DctDniz_Robust::setAlpha(double alpha)
{
	m_alpha = alpha;
}

void EBlindDLC_DctDniz_Robust::setDetectThreshold(double detectThreshold)
{
	m_detectThreshold = detectThreshold;
}

void EBlindDLC_DctDniz_Robust::setReference(std::weak_ptr<VideoFrame> preference)
{
	m_preference = preference;
}


bool EBlindDLC_DctDniz_Robust::embed(std::shared_ptr<VideoFrame> pframe, bool value)
{
	std::shared_ptr<VideoFrame> preferenceFrame = m_preference.lock();
	if (!preferenceFrame)
		return false;
	if (m_alpha < 0)
		return false;

	pframe->applyWR(preferenceFrame, m_alpha, value);

	return true;
}

EBlindDLC_DctDniz_Robust::DetectResult EBlindDLC_DctDniz_Robust::detect(const std::shared_ptr<VideoFrame> pframe)
{
	std::shared_ptr<VideoFrame> preferenceFrame = m_preference.lock();
	if (!preferenceFrame)
		return Watermark::FAILED;
	if (m_detectThreshold < 0)
		return Watermark::FAILED;

	Detector::Result res = Detector::Result::FAILED;

	if (pframe->colorFormat() == VideoFrame::Color)
		res = Detector::CorrelationCoefficientRGB(pframe, preferenceFrame, m_detectThreshold);
	else
		res = Detector::CorrelationCoefficientGrayscale(pframe, preferenceFrame, m_detectThreshold);

	return (Watermark::DetectResult) res;
}

std::shared_ptr<VideoFrame> EBlindDLC_DctDniz_Robust::createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue, float robustThreshold)
{
	auto preference =  WR::createRandom(pframe->width(), pframe->height(), maxValue, VideoFrame::Grayscale);
	preference->DCTSharpening(robustThreshold, maxValue);

	return preference;
}
