#include "stdafx.h"
#include "Encoder.h"
#include "Decoder.h"

int main() {

	
	Encoder myEncoder("C:\\shaked\\file\\old_farts.txt", "C:\\shaked\\file\\farts.dht");
	myEncoder.set_freaquencies();
	myEncoder.set_sub_intervals();
	myEncoder.set_properties();
	myEncoder.encode();
	

	Decoder myDecoder("C:\\shaked\\file\\farts.dht");
	size_t startPoint = myDecoder.retrieve_properties();
	myDecoder.decode(startPoint);
	system("PAUSE");
	return 0;
}