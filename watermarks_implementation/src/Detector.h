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
    PEARSON_COEFFICIENT = 3,
    CORRELATION_COEFFICIENT_LEGACY = 4
  };

  Result CorrelationCoefficientRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
  Result CorrelationCoefficientGrayscale(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
  template <typename T1, typename T2>
  Result CorrelationCoefficientLegacy(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation = NULL);
  template <typename T1, typename T2>
  Result CorrelationCoefficient(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation = NULL);

  Result NormalizedCorrelationRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation = NULL);
//  Result NormalizedCorrelationGrayscale(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold);

  template <typename T1, typename T2>
  Result LinearCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation = NULL);
  template <typename T1, typename T2>
  Result NormalizedCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation = NULL);
  template <typename T1, typename T2>
  Result PearsonCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation = NULL);

};


#endif