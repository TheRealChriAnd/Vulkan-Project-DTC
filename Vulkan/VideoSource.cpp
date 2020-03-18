#include "VideoSource.h"
#include "opencv2/opencv.hpp"
#include "ThreadManager.h"
#include "IVideoSourceListener.h"

VideoSource::VideoSource(const std::string& file) : 
	m_VideoCapture(nullptr),
	m_FrameCount(0),
	m_SleepTime(0),
	m_Width(0),
	m_Height(0),
	m_FrameSize(0)
{
	m_VideoCapture = new cv::VideoCapture(file);
	double fps = m_VideoCapture->get(cv::CAP_PROP_FPS);
	m_FrameCount = m_VideoCapture->get(cv::CAP_PROP_FRAME_COUNT);
	m_SleepTime = 1.0F / fps;

	if (!m_VideoCapture->isOpened())
		throw std::runtime_error("Error opening video stream or file!");

	m_Width = m_VideoCapture->get(cv::CAP_PROP_FRAME_WIDTH);
	m_Height = m_VideoCapture->get(cv::CAP_PROP_FRAME_HEIGHT);
	m_FrameSize = m_Width * m_Height * 4;

	m_PixelData = new unsigned char[m_FrameSize];
	memset(m_PixelData, 255, m_FrameSize);
}

VideoSource::~VideoSource()
{
	this->stop();
	m_VideoCapture->release();
	delete m_VideoCapture;
	delete[] m_PixelData;
}

void VideoSource::play()
{
	m_Playing = true;
	ThreadManager::addAsynchronousService(this);
}

void VideoSource::stop()
{
	m_Playing = false;
	ThreadManager::removeAsynchronousService(this);
}

int VideoSource::getHeight() const
{
	return m_Height;
}

int VideoSource::getWidth() const
{
	return m_Width;
}

unsigned const char* VideoSource::getFrameData() const
{
	return m_PixelData;
}

int VideoSource::getFrameSize() const
{
	return m_FrameSize;
}

void VideoSource::addListener(IVideoSourceListener* listener)
{
	m_Listeners.insert(listener);
}

void VideoSource::removeListener(IVideoSourceListener* listener)
{
	m_Listeners.erase(listener);
}

bool VideoSource::isPlaying() const
{
	return m_Playing;
}

void VideoSource::updateAsynchronous(float deltaSeconds)
{
	m_Timer += deltaSeconds;
	if (m_Timer < m_SleepTime)
		return;

	m_Timer -= m_SleepTime;
	m_CurrentFrame++;

	if (m_CurrentFrame == m_FrameCount)
		m_VideoCapture->set(cv::CAP_PROP_POS_FRAMES, 0);

	if (deltaSeconds > m_SleepTime)
	{
		m_VideoCapture->grab();
		return;
	}

	static cv::Mat frame;
	(*m_VideoCapture) >> frame;

	if (frame.empty())
		return;

	static int count = m_Width * m_Height * 3;
	int offset = 0;
	for (int i = 0; i < count; i += 3)
	{
		m_PixelData[i + offset] = frame.data[i + 2];
		m_PixelData[i + offset + 1] = frame.data[i + 1];
		m_PixelData[i + offset + 2] = frame.data[i];
		offset += 1;
	}

	for (IVideoSourceListener* listener : m_Listeners)
		listener->onFrameReady(this);
}
