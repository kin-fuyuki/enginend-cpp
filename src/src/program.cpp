#include "program.h"

long long calibrate() {
	unsigned long long start = __rdtsc();
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100000000L; // 0.1 seconds
	nanosleep(&ts, NULL);
	return (__rdtsc() - start) * 10;
}
const long long enginend::CPUCLOCK=calibrate();
void enginend::program::changescene(scene *scn) {
	this->currentscene->exit();
	delete this->currentscene;
	this->currentscene = scn;
	this->currentscene->boot();
}
