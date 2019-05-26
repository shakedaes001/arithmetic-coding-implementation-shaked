#pragma once
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include "Interval.h"
#include "SignToupleStructs.h"
#include <fstream>




//assist functions:
//
std::string toBinary(size_t n);

bool exists_in(const char sign, const std::vector<SignFreaquency>& list);

Interval find_in(const char sign, const std::vector<SignSubInterval>& list);
//



//class functions:
//
class Encoder {
private:
	std::vector<SignFreaquency> freaquencies;
	std::vector<SignSubInterval> subIntervals;

	std::ifstream inFile;
	std::string inFilePath;

	std::ofstream compressedFile;
	std::string compressedFilePath;

public:
	Encoder(std::string _inFilePath, std::string _compressedFilePath);

	void set_sub_intervals();

	void set_freaquencies();

	void send_bit(bool bit);

	void output_bits(bool outputBit, size_t& pendingBits);

	void encode();

	void set_properties();
};