#include "EBlindDLC.h"
#include "WatermarkReference.h"

EBlindDLC::EBlindDLC(double alpha, double detectThreshold, std::weak_ptr<VideoFrame> preference, Detector::Type detectorType): Watermark(),
	m_alpha(alpha),
	m_detectThreshold(detectThreshold),
	m_preference(preference),
	m_detectorType(detectorType)
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

	if (pframe->width() != preferenceFrame->width() || pframe->height() != preferenceFrame->height())
		return EBlindDLC::FAILED;


	Detector::Result res = Detector::Result::FAILED;

	std::size_t size = preferenceFrame->width() * preferenceFrame->height() * (pframe->colorFormat() == VideoFrame::Color ? 3 : 1);

	std::vector<double> data, noise;
	data.reserve(size);
	noise.reserve(size);

	uint8_t* pdata = pframe->data(0);
	uint8_t* pnoise = preferenceFrame->data(0);

	for (std::size_t i = 0; i < preferenceFrame->height(); i++)
	{
		data.insert(data.end(), pdata + i * pframe->stride(0), pdata + i * pframe->stride(0) + pframe->width());
		noise.insert(noise.end(), pnoise + i * preferenceFrame->stride(0), pnoise + i * preferenceFrame->stride(0) + preferenceFrame->width());
	}

	if (m_detectorType == Detector::CORRELATION_COEFFICIENT)
	{
		res = Detector::CorrelationCoefficient(data, noise, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::LINEAR)
	{
		res = Detector::LinearCorrelation(data, noise, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::NORMALIZED)
	{
		res = Detector::NormalizedCorrelation(data, noise, m_detectThreshold, pcorrelation);
	}
	else if (m_detectorType == Detector::PEARSON_COEFFICIENT)
	{
		res = Detector::PearsonCorrelation(data, noise, m_detectThreshold, pcorrelation);
	}

	return (EBlindDLC::DetectResult) res;
}

std::shared_ptr<VideoFrame> EBlindDLC::createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue)
{
	return WR::createRandom(pframe->width(), pframe->height(), maxValue, pframe->colorFormat());
}
