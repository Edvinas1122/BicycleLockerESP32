#ifndef LUTILS_H
# define LUTILS_H

# include <sys/time.h>
# include <Arduino.h>
# include "time.h"


const int64_t xx_time_get_time();
void printLocalTime();
void fetchLocalTime();
String getLocalTime();

#endif