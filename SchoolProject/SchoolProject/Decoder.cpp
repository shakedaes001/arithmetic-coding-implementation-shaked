#include "stdafx.h"
#include "Decoder.h"

Decoder::Decoder(std::string _compressedFilePath) {
	compressedFilePath = _compressedFilePath;
}

size_t Decoder::retrieve_properties() {
	compressedFile.open(compressedFilePath, std::ios::in | std::ios::binary);
	if (compressedFile.is_open()) {
		char originalFileType[3] = {'.', '.', '.'};
		size_t originalFileLength;
		size_t subIntervalsLength;
		SignSubInterval signInterval;
		size_t interStart;
		size_t interEnd;

		compressedFile.read(originalFileType, 3);

		outFilePath = compressedFilePath.substr(0, compressedFilePath.rfind('\.')) + '.' + originalFileType[0] + originalFileType[1] + originalFileType[2];

		std::cout << outFilePath << '\n';
		
		compressedFile.read((char*)&originalFileLength, sizeof(originalFileLength));

		std::cout << originalFileLength << '\n';

		compressedFile.read((char*)&subIntervalsLength, sizeof(subIntervalsLength));

		std::cout << subIntervalsLength << '\n';

		subIntervals.reserve(subIntervalsLength);

		for (size_t i = 0; i < subIntervalsLength; i++) {
			compressedFile.read((char*)&signInterval, sizeof(signInterval));

			subIntervals.push_back(signInterval);
		}

		for (size_t i = 0; i < subIntervals.size(); i++) {
			std::cout << subIntervals[i].to_string();
		}

		return (size_t)compressedFile.tellg();

		compressedFile.close();
	}
}

static char bitBlock = 0;
static int bitCounter = 8;

bool Decoder::get_bit() {
	if (bitCounter == 8) {
		bitCounter = 0;
		compressedFile.read(&bitBlock, sizeof(bitBlock));
	}

	if (bitBlock >= 128) {
		bitBlock <<= 1;
		bitCounter++;
		return true;
	}
	bitBlock <<= 1;
	bitCounter++;
	return false;

}

SignSubInterval Decoder::get_sign(size_t value, size_t range, size_t low) {
	for (size_t i = 0; i < subIntervals.size(); i++) {
		if ((low + (range * (subIntervals[i].inter.start / (double)subIntervals[i].inter.divider))) <= value && (low + (range * (subIntervals[i].inter.end / (double)subIntervals[i].inter.divider))) > value) {
			return subIntervals[i];
		}
	}
}

void Decoder::decode(size_t startPoint) {
	
	compressedFile.open(compressedFilePath, std::ios::in | std::ios::binary);
	outFile.open(outFilePath, std::ios::out | std::ios::binary);

	if (compressedFile.is_open() && outFile.is_open()) {

		size_t high = 0xFFFFFFFFU;
		size_t low = 0;
		size_t range;
		size_t value;
		SignSubInterval signInterval;

		compressedFile.seekg(startPoint, std::ios::beg);
		compressedFile.read((char*)&value, sizeof(value));
		
		size_t it = 0;
		while (it < subIntervals[0].inter.divider) {

			// set the upper and lower value to the current char
			range = high - low;

			signInterval = get_sign(value, range, low);
			outFile.write(&signInterval.sign, sizeof(signInterval.sign));

			high = low + (range * (signInterval.inter.end / (double)signInterval.inter.divider));
			low = low + (range * (signInterval.inter.start / (double)signInterval.inter.divider));
			
			it++;
			while (true) {
				// check if can extract bits from the high and low
				if (low >= 0x80000000U || high < 0x80000000U) {
					low <<= 1;
					high <<= 1;
					high |= 1;
					value <<= 1;
					value += get_bit() ? 1 : 0;
				}
				else if (low >= 0x40000000 && high < 0xC0000000U) {
					low <<= 1;
					low &= 0x7FFFFFFF;
					high <<= 1;
					high |= 0x80000001;
					value -= 0x4000000;
					value <<= 1;
					value += get_bit() ? 1 : 0;
				}
				else
					break;
			}
		}
	}
}