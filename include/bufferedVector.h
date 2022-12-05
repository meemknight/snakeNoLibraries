#pragma once
#define NOMINMAX
#include <Windows.h>

template<typename T, unsigned int N>
struct BufferedVector
{

	T data[N] = {};

	unsigned int size = 0;

	T &get(unsigned int index)
	{
		if (index >= size)
		{
			DebugBreak();
			ExitProcess(1);
		}

		return data[index];
	}

	T &last()
	{
		return get(size-1);
	}

	void push_back(const T &other)
	{
		if(size == N)
		{
			DebugBreak();
			ExitProcess(1);
		}

		data[size] = other;
		size++;
	}

	void pop_back()
	{
		if (size == 0)
		{
			DebugBreak();
			ExitProcess(1);
		}

		size--;
		data[size] = {};
	}



};