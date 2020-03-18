#pragma once

class VideoSource;

class IVideoSourceListener
{
public:
	virtual ~IVideoSourceListener() = default;
	virtual void onFrameReady(VideoSource* source) = 0;
};