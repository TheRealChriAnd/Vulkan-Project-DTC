#pragma once
#include <functional>
#include <string>
#include "IAsynchronous.h"
#include <set>
#include <atomic>

class IVideoSourceListener;

namespace cv
{
	class VideoCapture;
}

class VideoSource : public IAsynchronous
{
public:
	VideoSource(const std::string& file);
	virtual ~VideoSource();

	void play();
	void stop();
	bool isPlaying() const;

	int getHeight() const;
	int getWidth() const;

	unsigned const char* getFrameData() const;
	int getFrameSize() const;

	void addListener(IVideoSourceListener* listener);
	void removeListener(IVideoSourceListener* listener);

	virtual void updateAsynchronous(float deltaSeconds) override;

private:
	cv::VideoCapture* m_VideoCapture;

	double m_SleepTime;
	double m_Timer;
	int m_FrameSize;
	int m_Width;
	int m_Height;
	int m_FrameCount;
	int m_CurrentFrame;
	unsigned char* m_PixelData;
	std::atomic_bool m_Playing;
	std::set<IVideoSourceListener*> m_Listeners;

};

