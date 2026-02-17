#pragma once
#define CFG this->CONF()
#include <immintrin.h>
#include <time.h>

#include "scenes/scene.h"

namespace enginend{
	extern const long long CPUCLOCK;
inline const char* COMMONCONFIG();
class program {
	unsigned long long currenttick = __rdtsc();
	unsigned long long currentframe = __rdtsc();
public:
	scene *currentscene;
	int tickrate;
	int framerate;
	void changescene(scene*scn);
	program():client(false){}
	program(bool isclient):client(isclient){}
	virtual const char* CONF()=0;
	const bool client;
	virtual void boot()=0;
	virtual void tick()=0;
	virtual void draw()=0;
	virtual void exit()=0;
	bool shouldtick() {
		unsigned long long now = __rdtsc();
		unsigned long long interval = CPUCLOCK / tickrate;
		if (now - currenttick >= interval) {
			currenttick = now;
			return true;
		}
		return false;
	}

	bool shoulddraw() {
		unsigned long long now = __rdtsc();
		unsigned long long interval = CPUCLOCK / framerate;
		if (now - currentframe >= interval) {
			currentframe = now;
			return true;
		}
		return false;
	}
};}
