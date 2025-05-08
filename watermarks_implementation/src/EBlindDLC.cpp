#include "EBlindDLC.h"
#include "WatermarkReference.h"
#include "Detector.h"

EBlindDLC::EBlindDLC(double alpha, double detectThreshold, std::weak_ptr<VideoFrame> preference): Watermark(),
	m_alpha(alpha),
	m_detectThreshold(detectThreshold),
	m_preference(preference)
{
}

EBlindDLC::~EBlindDLC()
{
}

void EBlindDLC::setAlpha(double alpha)
{
	m_alpha = alpha;
}

void EBlindDLC::setDetectThreshold(double detectThreshold)
{
	m_detectThreshold = detectThreshold;
}

void EBlindDLC::setReference(std::weak_ptr<VideoFrame> preference)
{
	m_preference = preference;
}


bool EBlindDLC::embed(std::shared_ptr<VideoFrame> pframe, bool value)
{
	std::shared_ptr<VideoFrame> preferenceFrame = m_preference.lock();
	if (!preferenceFrame)
		return false;
	if (m_alpha < 0)
		return false;

	pframe->applyWR(preferenceFrame, m_alpha, value);

	return true;
}

EBlindDLC::DetectResult EBlindDLC::detect(const std::shared_ptr<VideoFrame> pframe)
{
	return detectInternal(pframe, NULL);
}

bool EBlindDLC::getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation)
{
	correlation = -1;
	detectInternal(pframe, &correlation);
	return true;
}

EBlindDLC::DetectResult EBlindDLC::detectInternal(const std::shared_ptr<VideoFrame> pframe, double* pcorrelation)
{
	std::shared_ptr<VideoFrame> preferenceFrame = m_preference.lock();
	if (!preferenceFrame)
		return EBlindDLC::FAILED;
	if (m_detectThreshold < 0)
		return EBlindDLC::FAILED;

	Detector::Result res = Detector::Result::FAILED;

	if (pframe->colorFormat() == VideoFrame::Color)
		res = Detector::CorrelationCoefficientRGB(pframe, preferenceFrame, m_detectThreshold, pcorrelation);
	else
		res = Detector::CorrelationCoefficientGrayscale(pframe, preferenceFrame, m_detectThreshold, pcorrelation);

	return (EBlindDLC::DetectResult) res;
}

std::shared_ptr<VideoFrame> EBlindDLC::createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue)
{
	return WR::createRandom(pframe->width(), pframe->height(), maxValue, pframe->colorFormat());
}
