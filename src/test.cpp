#include <incmgr.h>
#include <raylib.h>
#include <enginend/scenes/node2d.h>
using namespace enginend;
class launcher:public program {
public:
	
	bool vsync = true;
	scene s;
	const char* CONF() final{return "test.tdf";}
	launcher(){};
	void boot() override {
		SetConfigFlags(FLAG_VSYNC_HINT);
		InitWindow(500,500,"test");
		SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
		this->tickrate=GetMonitorRefreshRate(GetCurrentMonitor());
		s.nodes=std::list<nodes::node*>{
			
			new nodes::colored(Color{255,255,255,255},0,0,500,500),
			new nodes::textfield(nullptr,Color{255,127,127,255},Color{127,127,127,255}
								,100,100,220,32,GetFontDefault(),32,
								"welcome to enginend!\n"
								"hehe"
			)
		};
		s.boot();
		
	}
	void tick() override {
		if (shouldtick()) {
			s.tick();
		}
	}
	void draw() override {
		s.draw();
	}
	void exit() override {
		s.exit();
	}
};

tiny::ErrorLevel tiny::level{6};
int main(int argc, char *argv[]) {
	tiny::startup((char*)"enginend test",(char*)"1.0");
	launcher e;
	e.boot();
	while (!WindowShouldClose()) {
		e.tick();
		e.draw();
	}
	e.exit();
	return 0;
}