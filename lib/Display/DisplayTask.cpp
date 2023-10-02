#include "Display.h"

DisplayTask::DisplayTask(Display *display, TextSequence *sequence) {
	this->display = display;
	this->sequence = sequence;
}

void DisplayTask::start() {
	xTaskCreate(
		DisplayTask::task,
		"DisplayTask",
		10000,
		this,
		1,
		&this->taskHandle
	);
}

void DisplayTask::stop() {
	vTaskDelete(this->taskHandle);
}

void DisplayTask::task(void *parameter) {
	DisplayTask *displayTask = (DisplayTask *) parameter;
	Display *display = displayTask->display;
	TextSequence *sequence = displayTask->sequence;
	SequenceUnit unit;

	while(1) {
		unit = sequence->getSequenceUnit();
		display->displayText(unit.text);
		vTaskDelay(unit.duration / portTICK_PERIOD_MS);
	}
}