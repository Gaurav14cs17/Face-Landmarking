#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "../data/dataset.hpp"

using namespace cv;
using namespace std;
using namespace FaceLandmarking;

template<size_t N>
void example_test(experimental::filesystem::path dataPath)
{
	Data::Dataset<N> dataset(datePath);
	FaceLandmarking::FaceLandmarker<N> faceLandmarker(dataPath);

	namedWindow("example", WINDOW_AUTOSIZE);

	Mat imageWithMasks;

	for (auto iter = dataset.begin(); iter!= dataset.end(); iter++)
	{
		if (reader.hasNext()) {
			auto example = reader.loadNext();
			example.scaleFace(200, 200); // Maybe smaller? 

			FeatureExtraction::HsvImage processedImage;
			imagePreprocessor.processImage(example.image, processedImage, example.mask.faceRect(), false);
			maskRegression.setImage(processedImage);

			auto normalizedMask = MaskTransformation::MaskNormalizer<N>::normalizeMask(example.mask, Math::Rect<float>(Math::Point<float>(50, 50), Math::Size<float>(100, 100)));
			auto averageScaledMask = MaskTransformation::MaskNormalizer<N>::normalizeMask(averageMask, Math::Rect<float>(example.mask.faceCenter(), example.mask.faceSize()));

			auto adjustedMask = averageScaledMask;

			while (true)
			{
				for (int i = 0; i < N; i++)
				{
					adjustedMask[i] += maskRegression.computeOffset(adjustedMask[i], i, 10, 2);
				}
				adjustedMask = maskAutoencoder.passThrough(adjustedMask);

				example.image.copyTo(imageWithMasks);
				Test::UI::MaskUI<N>::drawMask(imageWithMasks, example.mask);
				Test::UI::MaskUI<N>::drawMask(imageWithMasks, averageScaledMask, cv::Scalar(0, 0, 255));
				Test::UI::MaskUI<N>::drawMask(imageWithMasks, adjustedMask, cv::Scalar(255, 255, 255));
				imshow("example", imageWithMasks);

				auto key = waitKey(250000);
				if (key == 32)
					continue;
				else if (key == 27)
					return;
				else
					break;
			}
		}
	}
}