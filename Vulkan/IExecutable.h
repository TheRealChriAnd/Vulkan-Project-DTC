#pragma once

class IExecutable
{
public:
	virtual ~IExecutable() {};

	virtual void execute() = 0;

protected:
	IExecutable() {};
};
