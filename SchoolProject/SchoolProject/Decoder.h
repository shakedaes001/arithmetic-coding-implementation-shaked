#pragma once
#include "stdafx.h"
#include "SignToupleStructs.h"
#include "Interval.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


class Decoder {
private:
	std::vector<SignSubInterval> subIntervals;

	std::ofstream outFile;
	std::string outFilePath;

	std::ifstream compressedFile;
	std::string compressedFilePath;

public:
	Decoder(std::string _compressedFilePath);

	size_t retrieve_properties();

	bool get_bit();

	SignSubInterval get_sign(size_t value, size_t range, size_t low);

	void decode(size_t startPoint);

};