#pragma once

class CameraVK;

class ICameraListener
{
public:
	virtual ~ICameraListener() = default;

	virtual void onCameraPathEnded(CameraVK* camera) = 0;
};