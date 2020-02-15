#pragma once

class IAsynchronous
{
public:
	virtual ~IAsynchronous() {};

	virtual void updateAsynchronous(float deltaSeconds) = 0;

protected:
	IAsynchronous() {};
};
