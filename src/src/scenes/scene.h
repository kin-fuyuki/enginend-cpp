#pragma once

#include "nodes.h"
#include <list>


namespace enginend {
	struct scene{
		std::list<enginend::nodes::node*> nodes;
		virtual void boot() {
			int i=0;
			tiny::echo((char*)"initializing scene");
			for (enginend::nodes::node* n : nodes) {
				tiny::echo((char*)"initializing object of ID: %i",i++);
				n->boot();
			}
		}
		virtual void draw() {
			
			ClearBackground(rl::BLANK);
			BeginDrawing();
			for (enginend::nodes::node* n : nodes) {
				n->draw();
			}
			EndDrawing();
		}
		virtual void tick() {
			for (enginend::nodes::node* n : nodes) {
				n->tick();
			}
		}
		virtual void exit() {
			for (enginend::nodes::node* n : nodes) {
				n->exit();
			}
		}
	};
}