#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <memory>
#include <vector>

class VideoFrame;

namespace Detector
{
  enum Result
  {
    FAILED,
    TRUE,
    FALSE,
    NO_WATERMARK
  };

  enum Type
  {
    LINEAR = 0,
    NORMALIZED = 1,
    CORRELATION_COEFFICIENT = 2,
    PEARSON_COEFFICIENT = 3
  };

  Result CorrelationCoefficientRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
  Result CorrelationCoefficientGrayscale(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
  Result CorrelationCoefficientLegacy(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation = NULL);
  Result CorrelationCoefficient(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation = NULL);

  Result LinearCorrelation(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation = NULL);

  Result NormalizedCorrelationRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
//  Result NormalizedCorrelationGrayscale(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold);
  Result NormalizedCorrelation(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation = NULL);
  Result PearsonCorrelation(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation = NULL);
};


#endif