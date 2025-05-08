#ifndef DCT_WATERMARK_H_
#define DCT_WATERMARK_H_

#include "Watermark.h"
#include "Detector.h"

class DCTWatermark : public Watermark
{
public:
	enum WatermarkType
	{
		DEFAULT,
		BARNI,
		LEGACY_CC
	};

	enum EmbeddingType
	{
		SKIP_FIRST,
		MIDDLE_FREQUENCY
	};

	DCTWatermark(double alpha = -1, double detectThreshold = -1, int skipCount = -1, const std::vector<int> reference = std::vector<int>(), WatermarkType type = DEFAULT, Detector::Type detectorType = Detector::CORRELATION_COEFFICIENT, EmbeddingType embeddingType = EmbeddingType::SKIP_FIRST);
	void setAlpha(double alpha);
	void setSkipCount(int skipCount);
	void setDetectThreshold(double detectThreshold);
	void setReference(const std::vector<int>& reference);

	bool embed(std::shared_ptr<VideoFrame> pframe, bool value);
	DetectResult detect(const std::shared_ptr<VideoFrame> pframe);
	bool getCorrelation(const std::shared_ptr<VideoFrame> pframe, double& correlation);
	bool getAverageDCT(const std::shared_ptr<VideoFrame> pframe, double& averageDCT);

	static std::vector<int> createReference(const std::shared_ptr<VideoFrame> pframe, int maxValue, int skipCount, bool BarniVersion = false, EmbeddingType embeddingType = EmbeddingType::SKIP_FIRST);
	static std::vector<int> cropReference(const std::vector<int> &reference, int skipCount, std::size_t x, std::size_t y, std::size_t width, std::size_t height, std::size_t widthOrig, std::size_t heightOrig);
	static void referenceStore(const std::vector<int>& data, const std::string& filename);
	static std::vector<int> referenceLoad(const std::string& filename);

	static const int BLOCK_SIZE = 8;
private:
	DetectResult detectInternal(const std::shared_ptr<VideoFrame> pframe, double *pcorrelation, double *averageDCT);


	double                       m_alpha;
	double                       m_detectThreshold;
	int                          m_skipCount;
	std::vector<int>             m_reference;
	Detector::Type               m_detectorType;
	WatermarkType                m_watermarkType;
	DCTWatermark::EmbeddingType  m_embeddingType;

};

#endif
