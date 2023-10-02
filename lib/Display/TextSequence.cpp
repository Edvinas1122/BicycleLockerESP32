#include "Display.h"

TextSequence::TextSequence(SequenceUnit *sequence, int sequenceLength) {
	this->sequence = sequence;
	this->sequenceLength = sequenceLength;
	this->currentUnit = 0;
}

SequenceUnit TextSequence::getSequenceUnit() {
	SequenceUnit unit = this->sequence[this->currentUnit];
	this->currentUnit = (this->currentUnit + 1) % this->sequenceLength;
	return unit;
}