#pragma once

#include "net.h"
#include "gr.h"
#include "aud.h"
#include "program.h"
#include "scenes/scene.h"
enum PLATFORM {
	WINDOWS,LINUX,MACOS,
	ANDROID,IOS
};
extern PLATFORM platform;
extern std::string androidpackage;
#include "resmgr.h"