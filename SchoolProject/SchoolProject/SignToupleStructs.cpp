#include "stdafx.h"
#include "SignToupleStructs.h"

SignFreaquency::SignFreaquency(char _sign) {
	sign = _sign;
	freaquency = 1;
}

void SignFreaquency::add_freaquency() {
	freaquency++;
}

std::string SignFreaquency::to_string() {
	return std::string("sign: ") + sign + std::string(", freaquency: ") + std::to_string(freaquency) + ".\n";
}

SignSubInterval::SignSubInterval() {}

SignSubInterval::SignSubInterval(char _sign, Interval _inter) {
	sign = _sign;
	inter = _inter;
}

std::string SignSubInterval::to_string() {
	return std::string("sign: ") + sign + std::string(", sub interval: ") + inter.to_string() + ".\n";
}