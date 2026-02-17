#pragma once
#include <string>
#include "engine.h"
inline const char *AT(std::string path) {
	if (platform==LINUX || platform==WINDOWS || platform==MACOS) {
		return path.c_str();
	}else {
		if (platform==ANDROID) {
			return ("/data/data/"+androidpackage+"/files/"+path).c_str();
		}
	}
}
