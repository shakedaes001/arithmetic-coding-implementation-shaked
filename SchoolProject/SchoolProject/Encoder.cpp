#include "stdafx.h"
#include "Encoder.h"

static int bitCounter = 0;
static char bitBlock = 0;

std::string toBinary(size_t n)
{
	std::string r;
	int laps = 32;
	while (n != 0) { r = (n % 2 == 0 ? "0" : "1") + r; n /= 2; laps--; }
	for (laps; laps > 0; laps--) { r = "0" + r; }
	return r;
}

bool exists_in(const char sign, const std::vector<SignFreaquency>& list) {
	for (std::vector<SignFreaquency>::size_type i = 0; i < list.size(); i++) {
		if (list[i].sign == sign) return true;
	}
	return false;
}

Interval find_in(const char sign, const std::vector<SignSubInterval>& list) {
	for (std::vector<SignSubInterval>::size_type i = 0; i < list.size(); i++) {
		if (list[i].sign == sign) return list[i].inter;
	}
	return Interval();
}


Encoder::Encoder(std::string _inFilePath, std::string _compressedFilePath) {
	inFilePath = _inFilePath;
	compressedFilePath = _compressedFilePath;
}

void Encoder::set_sub_intervals() {
	size_t midPoint = 0;

	// get the size of the file
	inFile.open(inFilePath, std::ios::binary);
	if (inFile.is_open()) {
		inFile.seekg(0, std::ios::end);
		size_t fileSize = inFile.tellg();
		inFile.close();

		for (int i = 0; i < freaquencies.size(); i++) {
			subIntervals.push_back(SignSubInterval(freaquencies[i].sign, Interval(midPoint, midPoint + freaquencies[i].freaquency, fileSize)));
			midPoint += freaquencies[i].freaquency;
		}
	}
}

void Encoder::set_freaquencies() {
	// function gets empty vector of probabilities and a string and find the probabilities of each char in the text and place it in the vector.

	inFile.open(inFilePath, std::ios::binary);

	if (inFile.is_open()) {
		char currLetter;
		inFile.seekg(0, std::ios::beg);
		while (inFile.read(&currLetter, sizeof(currLetter))) {
			if (exists_in(currLetter, freaquencies)) {
				for (size_t i = 0; i < freaquencies.size(); i++) {
					if (freaquencies[i].sign == currLetter) {
						freaquencies[i].add_freaquency();
						break;
					}
				}
			}
			else freaquencies.push_back(SignFreaquency(currLetter));
		}
		inFile.close();

	}
}

void Encoder::send_bit(bool bit) {
	bitCounter++;
	if (bit) { bitBlock <<= 1; bitBlock++; }
	else bitBlock <<= 1;
	if (bitCounter == 8) {
		std::cout << bitBlock;
		compressedFile.write((char*)&bitBlock, sizeof(bitBlock));
		bitCounter = 0;
		bitBlock = 0;
	}
} // need fixing at the last bit if it not add up to 8 bits then the bits at the end will not be saved.

void Encoder::output_bits(bool outputBit, size_t& pendingBits) {
	// send the bit to the file
	send_bit(outputBit);
	while (pendingBits) {
		send_bit(!outputBit);
		pendingBits--;
	}
}

void Encoder::encode() {
	inFile.open(inFilePath, std::ios::binary);
	compressedFile.open(compressedFilePath, std::ios::app | std::ios::ate);


	if (inFile.is_open() && compressedFile.is_open()) {

		size_t iterationsSum = 0;

		size_t high = 0xFFFFFFFFU;
		size_t low = 0;
		size_t range;
		Interval signInterval;
		size_t pendingBits = 0;
		int iterations = 0;
		char currLetter;
		inFile.seekg(0, std::ios::beg);
		while (inFile.read(&currLetter, sizeof(currLetter))) {
			iterationsSum += iterations;
			iterations = 0;

			// set the upper and lower value to the current char
			range = high - low;
			signInterval = find_in(currLetter, subIntervals);
			high = low + (range * (signInterval.end / (double)signInterval.divider));
			low = low + (range * (signInterval.start / (double)signInterval.divider));
			while (true) {
				// check if can extract bits from the high and low
				if (high < 0x80000000U) output_bits(false, pendingBits);

				else if (low >= 0x80000000) output_bits(true, pendingBits);

				else if (high < 0xC0000000U && low >= 0x40000000) {
					pendingBits++;
					high <<= 1;
					high |= 0x80000001;
					low <<= 1;
					low &= 0x7FFFFFFF;
					iterations++;
					continue;
				}
				else break;
				iterations++;

				// output_bits(outputBit, pendingBits);
				// take out the leftest bit when identical at both high and low
				high <<= 1;
				low <<= 1;
				high++;
			}
		}

		for (size_t i = 0; i < 32; i++) {
			if (low >= 0x80000000) send_bit(true);
			else send_bit(false);
			low <<= 1;
		}

		if (bitCounter != 0) {
			for (size_t i = 0; i < (8 - bitCounter); i++) {
				send_bit(true);
			}
		}

		compressedFile.close();
		inFile.close();

		double avg;

		inFile.open(inFilePath, std::ios::binary);
		if (inFile.is_open()) {
			inFile.seekg(0, std::ios::end);
			avg = ((double)iterationsSum / ((size_t)inFile.tellg())) / 8;
		}

		std::cout << avg << '\n';
	}
}


void Encoder::set_properties()
{
	// write to compressed file the original file type, original file length, subintervals list length and subintervals list in that order.

	size_t inFileSize;
	size_t subIntervalsLength;
	inFile.open(inFilePath, std::ios::binary);
	if (inFile.is_open()) {
		inFile.seekg(0, std::ios::end);
		inFileSize = (size_t)inFile.tellg();
		inFile.close();
	}
	subIntervalsLength = (size_t)subIntervals.size();

	std::string inFileType = inFilePath.substr(inFilePath.rfind('\.') + 1, inFilePath.size() - 1); // fix this..
	
	compressedFile.open(compressedFilePath, std::ios::binary);
	if (compressedFile.is_open()) {

		// write to file here
		compressedFile << inFileType;
		compressedFile.write((char*)&inFileSize, sizeof(inFileSize));
		compressedFile.write((char*)&subIntervalsLength, sizeof(subIntervalsLength));
		for (size_t i = 0; i < subIntervalsLength; i++) { // write the subIntervals in the file: sign > interval.start > interval.end
			compressedFile.write((char*)&subIntervals[i], sizeof(subIntervals[i]));
		}
		compressedFile.close();
	}
}