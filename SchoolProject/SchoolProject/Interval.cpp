#include "stdafx.h"
#include "Interval.h"

Interval::Interval() {
	start = 0;
	end = 0;
	divider = 1;
}

Interval::Interval(size_t _start, size_t _end, size_t _divider) {
	start = _start;
	end = _end;
	divider = _divider;
}

void operator << (std::ostream os, Interval& inter) {
	os << inter.to_string();
}

std::string Interval::to_string() {
	return "[" + std::to_string(start) + ", " + std::to_string(end) + ", " + std::to_string(divider) + ")";
}