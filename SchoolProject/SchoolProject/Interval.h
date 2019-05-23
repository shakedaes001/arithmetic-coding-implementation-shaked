#pragma once
#include <ostream>
#include <string>

class Interval {
public:
	size_t start, end, divider;

	Interval();

	Interval(size_t _start, size_t _end, size_t _divider);

	friend void operator << (std::ostream os, Interval& inter);

	std::string to_string();
};