#include "Stdafx.h"
#include "IR16FrameEventAdapter.h"
#include <functional>
#include <queue>
#include <memory>

IR16Filters::IR16FrameEventAdapter::IR16FrameEventAdapter(IR16Filters::NewIR16FrameEvent^ previewHandler, IR16Filters::NewIR16FrameEvent^ rawHandler) :
	mPreviewHandler(previewHandler), mRawHandler(rawHandler), mAdaptiveClampingAGC(true), mMinMaxAGC(false), mClampingAGC(false)
{
}

void IR16Filters::IR16FrameEventAdapter::NotifyFrame(uint16_t* buffer, int width, int height) {
	if (mPreviewHandler != nullptr) {
		//Generate a bitmap for preview data
		System::Drawing::Bitmap^ previewImg = gcnew System::Drawing::Bitmap(width, height,
			System::Drawing::Imaging::PixelFormat::Format24bppRgb);
		System::Drawing::Imaging::BitmapData^ data = previewImg->LockBits(
			System::Drawing::Rectangle(0, 0, width, height),
			System::Drawing::Imaging::ImageLockMode::ReadWrite,
			System::Drawing::Imaging::PixelFormat::Format24bppRgb);

		//Create an array to copy preview data into. 24bpp
		array<uint8_t>^ previewArr = gcnew array<uint8_t>(width * height * 3);

		//Do AGC to generate the preview image.
		DoAGC(buffer, previewArr, width * height);
		System::Runtime::InteropServices::Marshal::Copy(previewArr, 0, data->Scan0, previewArr->Length);
		previewImg->UnlockBits(data);

		mPreviewHandler->Invoke(previewImg);
	}

	if (mRawHandler != nullptr) {
		//Generate a bitmap for raw data
		System::Drawing::Bitmap^ rawImg = gcnew System::Drawing::Bitmap(width, height,
			System::Drawing::Imaging::PixelFormat::Format16bppRgb565);
		System::Drawing::Imaging::BitmapData^ data = rawImg->LockBits(
			System::Drawing::Rectangle(0, 0, width, height),
			System::Drawing::Imaging::ImageLockMode::ReadWrite,
			System::Drawing::Imaging::PixelFormat::Format16bppRgb565);

		//Create an array to copy raw data into. 16bpp
		array<uint8_t>^ rawArr = gcnew array<uint8_t>(width * height * 2);

		//Pin the ptr in this scope to prevent it from being moved around by the garbage collector, then copy data.
		pin_ptr<uint8_t> pinPtrRawArr = &rawArr[rawArr->GetLowerBound(0)];
		memcpy(pinPtrRawArr, (uint8_t*)buffer, width * height * 2);

		System::Runtime::InteropServices::Marshal::Copy(rawArr, 0, data->Scan0, rawArr->Length);

		rawImg->UnlockBits(data);

		mRawHandler->Invoke(rawImg);
	}
}

void IR16Filters::IR16FrameEventAdapter::Stop() {
	mPreviewHandler = nullptr;
	mRawHandler = nullptr;
}

void IR16Filters::IR16FrameEventAdapter::DoAGC(uint16_t* bufferIn, array<uint8_t>^ bufferOut, int numPixels) {
	if (mAdaptiveClampingAGC) {
		const double chopFraction = 0.0005;
		const int chopSize = (int)(min(100, chopFraction * numPixels));

		std::priority_queue<uint16_t> lowest;
		std::priority_queue<uint16_t, std::vector<uint16_t>, std::greater<uint16_t> > highest =
			std::priority_queue<uint16_t, std::vector<uint16_t>, std::greater<uint16_t> >();

		int i = 0;
		for (; i < chopSize; i++) {
			lowest.push(bufferIn[i]);
			highest.push(bufferIn[i]);
		}

		uint16_t lowestCached = lowest.top();
		uint16_t highestCached = highest.top();

		for (; i < numPixels; i++) {
			if (bufferIn[i] < lowestCached) {
				lowest.pop();
				lowest.push(bufferIn[i]);
				lowestCached = lowest.top();
			}
			if (bufferIn[i] > highestCached) {
				highest.pop();
				highest.push(bufferIn[i]);
				highestCached = highest.top();
			}
		}

		int clampLow = lowestCached;
		int clampHigh = highestCached;

		for (int i = 0; i < numPixels; i++) {
			uint8_t clampedVal = 0;
			if (bufferIn[i] > clampHigh) {
				clampedVal = 255;
			} else if (bufferIn[i] < clampLow) {
				clampedVal = 0;
			} else {
				double val = ((double)bufferIn[i] - clampLow) / (abs(clampHigh - clampLow) + 1) * 255;
				clampedVal = (uint8_t)val;
			}
			bufferOut[3 * i + 0] = clampedVal;
			bufferOut[3 * i + 1] = clampedVal;
			bufferOut[3 * i + 2] = clampedVal;
		}
	}
	else if (mMinMaxAGC) {
		int frameMin = 0xFFFF;
		int frameMax = 0;
		for (int i = 0; i < numPixels; i++) {
			frameMin = min(frameMin, bufferIn[i]);
			frameMax = max(frameMax, bufferIn[i]);
		}
		for (int i = 0; i < numPixels; i++) {
			uint8_t val = (uint8_t)(((double)bufferIn[i] - frameMin) / (frameMax - frameMin) * 255);
			bufferOut[3 * i + 0] = val;
			bufferOut[3 * i + 1] = val;
			bufferOut[3 * i + 2] = val;
		}
	}
	else if (mClampingAGC) {
		for (int i = 0; i < numPixels; i++) {
			double val = ((double)bufferIn[i] - mClampLow) / (mClampHigh - mClampLow) * 255;
			uint8_t clampedVal = (uint8_t) val;
			if (val > 255) {
				clampedVal = 255;
			} else if (val < 0) {
				clampedVal = 0;
			}
			bufferOut[3 * i + 0] = clampedVal;
			bufferOut[3 * i + 1] = clampedVal;
			bufferOut[3 * i + 2] = clampedVal;
		}
	}
}

void IR16Filters::IR16FrameEventAdapter::UseAdaptiveClampingAGC() {
	mAdaptiveClampingAGC = true;
	mMinMaxAGC = false;
	mClampingAGC = false;
}

void IR16Filters::IR16FrameEventAdapter::UseMinMaxAGC() {
	mAdaptiveClampingAGC = false;
	mMinMaxAGC = true;
	mClampingAGC = false;
}
void IR16Filters::IR16FrameEventAdapter::UseClampingAGC(int min, int max) {
	mAdaptiveClampingAGC = false;
	mMinMaxAGC = false;
	mClampingAGC = true;
	mClampLow = min;
	mClampHigh = max;
}