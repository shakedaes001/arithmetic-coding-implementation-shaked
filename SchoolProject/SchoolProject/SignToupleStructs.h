#pragma once
#include "Interval.h"

struct SignFreaquency {
	char sign;
	size_t freaquency;

	SignFreaquency(char _sign);

	void add_freaquency();

	std::string to_string();
};

struct SignSubInterval {
	char sign;
	Interval inter;

	SignSubInterval();

	SignSubInterval(char _sign, Interval _inter);

	std::string to_string();
};