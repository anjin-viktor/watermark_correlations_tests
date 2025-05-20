#include "Detector.h"
#include "VideoFrame.h"

#include <cmath>

Detector::Result Detector::CorrelationCoefficientRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation)
{
  if (!pFrame || !pFrameNoise)
    return Detector::FAILED;

  if(pFrame->width() != pFrameNoise->width() || pFrame->height() != pFrameNoise->height())
    return Detector::FAILED;

  uint8_t* pdata = pFrame->data(0);
  uint8_t* pnoise = pFrameNoise->data(0);

  std::size_t width = pFrame->width();
  std::size_t stride = width * 3;
  std::size_t height = pFrame->height();
  int result = 0;

  double meanRf = 0, meanRn = 0, meanGf = 0, meanGn = 0, meanBf = 0, meanBn = 0;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      meanBf += pdata[i * stride + j * 3];
      meanBn += pnoise[i * stride + j * 3];

      meanGf += pdata[i * stride + j * 3 + 1];
      meanGn += pnoise[i * stride + j * 3 + 1];

      meanRf += pdata[i * stride + j * 3 + 2];
      meanRn += pnoise[i * stride + j * 3 + 2];
    }
  }

  meanBf /= height * width;
  meanBn /= height * width;
  meanGf /= height * width;
  meanGn /= height * width;
  meanRf /= height * width;
  meanRn /= height * width;

  double numB = 0, sqrBf = 0, sqrBn = 0;
  double numG = 0, sqrGf = 0, sqrGn = 0;
  double numR = 0, sqrRf = 0, sqrRn = 0;


  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      numB += ((double)pdata[i * stride + j * 3] - meanBf) * ((double)pnoise[i * stride + j * 3] - meanBn);
      numG += ((double)pdata[i * stride + j * 3 + 1] - meanGf) * ((double)pnoise[i * stride + j * 3 + 1] - meanGn);
      numR += ((double)pdata[i * stride + j * 3 + 2] - meanRf) * ((double)pnoise[i * stride + j * 3 + 2] - meanRn);

      sqrBf += std::pow(pdata[i * stride + j * 3] - meanBf, 2);
      sqrBn += std::pow(pnoise[i * stride + j * 3] - meanBn, 2);

      sqrGf += std::pow(pdata[i * stride + j * 3 + 1] - meanGf, 2);
      sqrGn += std::pow(pnoise[i * stride + j * 3 + 1] - meanGn, 2);

      sqrRf += std::pow(pdata[i * stride + j * 3 + 2] - meanRf, 2);
      sqrRn += std::pow(pnoise[i * stride + j * 3 + 2] - meanRn, 2);
    }
  }

  double corrB = numB / std::sqrt(sqrBf) / std::sqrt(sqrBn);
  double corrG = numG / std::sqrt(sqrGf) / std::sqrt(sqrGn);
  double corrR = numR / std::sqrt(sqrRf) / std::sqrt(sqrRn);

  double corr = (corrB + corrG + corrR) / 3;
  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if(corr > threshold)
    res = Detector::TRUE;
   
  return res;
}

Detector::Result Detector::CorrelationCoefficientGrayscale(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double* pcorrelation)
{
  if (!pFrame || !pFrameNoise)
    return Detector::FAILED;

  if (pFrame->width() != pFrameNoise->width() || pFrame->height() != pFrameNoise->height())
    return Detector::FAILED;

  uint8_t* pdata = pFrame->data(0);
  uint8_t* pnoise = pFrameNoise->data(0);

  std::size_t width = pFrame->width();
  std::size_t stride = pFrame->stride(0);
  std::size_t height = pFrame->height();
  int result = 0;

  double meanf = 0, meann = 0;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      meanf += pdata[i * stride + j];
      meann += pnoise[i * stride + j];
    }
  }

  meanf /= height * width;
  meann /= height * width;

  double num = 0, sqrf = 0, sqrn = 0;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      num += ((double)pdata[i * stride + j] - meanf) * ((double)pnoise[i * stride + j] - meann);

      sqrf += std::pow(pdata[i * stride + j] - meanf, 2);
      sqrn += std::pow(pnoise[i * stride + j] - meann, 2);
    }
  }

  double corr = num / std::sqrt(sqrf) / std::sqrt(sqrn);
  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

template <typename T1, typename T2>
Detector::Result Detector::CorrelationCoefficient(std::vector<T1> data, std::vector<T2> noise, double threshold, double *pcorrelation)
{
  if (data.size() != noise.size() || data.empty())
    return Detector::FAILED;

  double meand = 0, meann = 0;
  for (std::size_t i = 0; i < data.size(); i++)
  {
      meand += data[i];
      meann += noise[i];
  }

  meand /= data.size();
  meann /= data.size();

  for (std::size_t i = 0; i < data.size(); i++)
  {
    data[i] -= meand;
    noise[i] -= meann;
  }

  return NormalizedCorrelation(data, noise, threshold, pcorrelation);
}

template <typename T1, typename T2>
Detector::Result Detector::CorrelationCoefficientLegacy(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation)
{
  if (data.size() != noise.size())
    return Detector::FAILED;

  double meand = 0, meann = 0;
  for (int i = 0; i < data.size(); i++)
  {
    meand += data[i];
    meann += noise[i];
  }

  meand /= data.size();
  meann /= data.size();

  double num = 0, sqrf = 0, sqrn = 0;

  for (int i = 0; i < data.size(); i++)
  {
    num += ((double)data[i] - meand) * ((double)noise[i] - meann);

    sqrf += std::pow(data[i] - meand, 2);
    sqrn += std::pow(noise[i] - meann, 2);
  }

  double corr = num / std::sqrt(sqrf) / std::sqrt(sqrn);
  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

Detector::Result Detector::NormalizedCorrelationRGB(std::shared_ptr<VideoFrame> pFrame, std::shared_ptr<VideoFrame> pFrameNoise, double threshold, double *pcorrelation) // This function produces wrong results becauses uses CorrelationCoeffecients instead of LinearCorrelation
{
  if (!pFrame || !pFrameNoise)
    return Detector::FAILED;

  if (pFrame->width() != pFrameNoise->width() || pFrame->height() != pFrameNoise->height())
    return Detector::FAILED;

  uint8_t* pdata = pFrame->data(0);
  uint8_t* pnoise = pFrameNoise->data(0);

  std::size_t width = pFrame->width();
  std::size_t stride = width * 3;
  std::size_t height = pFrame->height();
  int result = 0;

  double normBf = 0, normBn = 0, normGf = 0, normGn = 0, normRf = 0, normRn = 0;

  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      normBf += std::pow(pdata[i * stride + j * 3], 2) ;
      normBn += std::pow(pnoise[i * stride + j * 3], 2);

      normGf += std::pow(pdata[i * stride + j * 3 + 1], 2);
      normGn += std::pow(pnoise[i * stride + j * 3 + 1], 2);

      normRf += std::pow(pdata[i * stride + j * 3 + 2], 2);
      normRn += std::pow(pnoise[i * stride + j * 3 + 2], 2);
    }
  }

  normBf = std::sqrt(normBf);
  normBn = std::sqrt(normBn);
  normGf = std::sqrt(normGf);
  normGn = std::sqrt(normGn);
  normRf = std::sqrt(normRf);
  normRn = std::sqrt(normRn);

  if (normBf == 0 || normBn == 0 || normGf == 0 || normGn == 0 || normRf == 0 || normRn == 0) //use linear corelation instead of normalized
  {
    normBf = 1;
    normBn = 1;
    normGf = 1;
    normGn = 1;
    normRf = 1;
    normRn = 1;
  }

  double meanRf = 0, meanRn = 0, meanGf = 0, meanGn = 0, meanBf = 0, meanBn = 0;
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      meanBf += pdata[i * stride + j * 3] / normBf;
      meanBn += pnoise[i * stride + j * 3] / normBn;

      meanGf += pdata[i * stride + j * 3 + 1] / normGf;
      meanGn += pnoise[i * stride + j * 3 + 1] / normGn;

      meanRf += pdata[i * stride + j * 3 + 2] / normRf;
      meanRn += pnoise[i * stride + j * 3 + 2] / normRn;
    }
  }

  meanBf /= height * width;
  meanBn /= height * width;
  meanGf /= height * width;
  meanGn /= height * width;
  meanRf /= height * width;
  meanRn /= height * width;

  double numB = 0, sqrBf = 0, sqrBn = 0;
  double numG = 0, sqrGf = 0, sqrGn = 0;
  double numR = 0, sqrRf = 0, sqrRn = 0;


  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      numB += ((double)pdata[i * stride + j * 3] / normBf - meanBf) * ((double)pnoise[i * stride + j * 3] / normBn - meanBn);
      numG += ((double)pdata[i * stride + j * 3 + 1] / normGf - meanGf) * ((double)pnoise[i * stride + j * 3 + 1] / normGn - meanGn);
      numR += ((double)pdata[i * stride + j * 3 + 2] / normRf - meanRf) * ((double)pnoise[i * stride + j * 3 + 2] / normRn - meanRn);

      sqrBf += std::pow(pdata[i * stride + j * 3] / normBf - meanBf, 2);
      sqrBn += std::pow(pnoise[i * stride + j * 3] / normBn - meanBn, 2);

      sqrGf += std::pow(pdata[i * stride + j * 3 + 1] / normGf - meanGf, 2);
      sqrGn += std::pow(pnoise[i * stride + j * 3 + 1] / normGn - meanGn, 2);

      sqrRf += std::pow(pdata[i * stride + j * 3 + 2] / normRf - meanRf, 2);
      sqrRn += std::pow(pnoise[i * stride + j * 3 + 2] / normRn - meanRn, 2);
    }
  }

  double corrB = 0;
  if(sqrBf != 0 && sqrBn != 0)
    corrB = numB / std::sqrt(sqrBf) / std::sqrt(sqrBn);
  double corrG = 0;
  if (sqrGf != 0 && sqrGn != 0)
    corrG = numG / std::sqrt(sqrGf) / std::sqrt(sqrGn);
  double corrR = 0;
  if (sqrRf != 0 && sqrRn != 0)
    corrR = numR / std::sqrt(sqrRf) / std::sqrt(sqrRn);

  double corr = (corrB + corrG + corrR) / 3;
  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

template <typename T1, typename T2>
Detector::Result Detector::LinearCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation)
{
  if (data.size() != noise.size())
    return Detector::FAILED;

  double corr = 0;
  for (std::size_t i = 0; i < data.size(); i++)
  {
    corr += data[i] * noise[i];
  }

  corr /= data.size();

  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

template <typename T1, typename T2>
Detector::Result Detector::NormalizedCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation)
{
  if (data.size() != noise.size() || data.empty())
    return Detector::FAILED;

  double dataNorm = 0;
  double noiseNorm = 0;

  double corr = 0;

  for (std::size_t i = 0; i < data.size(); i++)
  {
    dataNorm += data[i] * data[i];
    noiseNorm += noise[i] * noise[i];
  }

  dataNorm = sqrt(dataNorm);
  noiseNorm = sqrt(noiseNorm);

  for (std::size_t i = 0; i < data.size(); i++)
    corr += data[i] / dataNorm * noise[i] / noiseNorm;

  corr /= data.size();

  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

template <typename T1, typename T2>
Detector::Result Detector::PearsonCorrelation(std::vector<T1> data, std::vector<T2> noise, double threshold, double* pcorrelation)
{
  if (data.size() != noise.size() || data.empty())
    return Detector::FAILED;

  double meand = 0, meann = 0;
  for (std::size_t i = 0; i < data.size(); i++)
  {
    meand += data[i];
    meann += noise[i];
  }

  meand /= data.size();
  meann /= data.size();

  double cov = 0, stdD = 0, stdN = 0;
  for (std::size_t i = 0; i < data.size(); i++)
  {
    double diffD = data[i] - meand;
    double diffN = noise[i] - meann;
    cov += diffD * diffN;
    stdD += diffD * diffD;
    stdN += diffN * diffN;
  }

  if (stdD == 0.0 || stdN == 0.0) 
  {
    if (pcorrelation)
      *pcorrelation = 0;

    return Detector::NO_WATERMARK;
  }

  double corr = cov / (std::sqrt(stdD) * std::sqrt(stdN));

  if (pcorrelation)
    *pcorrelation = corr;

  Detector::Result res = Detector::NO_WATERMARK;
  if (corr < -threshold)
    res = Detector::FALSE;
  else if (corr > threshold)
    res = Detector::TRUE;

  return res;
}

template Detector::Result Detector::CorrelationCoefficientLegacy<int,int>(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::CorrelationCoefficientLegacy<int,double>(std::vector<int> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::CorrelationCoefficientLegacy<double, double>(std::vector<double> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::CorrelationCoefficient<int, int>(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::CorrelationCoefficient<int, double>(std::vector<int> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::CorrelationCoefficient<double, double>(std::vector<double> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::LinearCorrelation<int, int>(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::LinearCorrelation<int, double>(std::vector<int> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::LinearCorrelation<double, double>(std::vector<double> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::NormalizedCorrelation<int, int>(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::NormalizedCorrelation<int, double>(std::vector<int> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::NormalizedCorrelation<double, double>(std::vector<double> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::PearsonCorrelation<int, int>(std::vector<int> data, std::vector<int> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::PearsonCorrelation<int, double>(std::vector<int> data, std::vector<double> noise, double threshold, double* pcorrelation);
template Detector::Result Detector::PearsonCorrelation<double, double>(std::vector<double> data, std::vector<double> noise, double threshold, double* pcorrelation);
