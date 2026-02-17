#pragma once
#include <vector>

#include "../gr.h"
#include "../aud.h"
#include "../net.h"
#include<tiny/term.h>

namespace enginend {
	namespace nodes {
		struct node{
		public:
			virtual void boot()=0;
			virtual void tick()=0;
			virtual void draw()=0;
			virtual void exit()=0;
		};
	}
	struct group : public virtual enginend::nodes::node {
		std::vector<node*> children;
		group(std::vector<node*>& children) : children(std::move(children)) {}
		void boot(){for (node* n: children){n->boot();}}
		void tick(){for (node* n: children){n->tick();}}
		void draw(){for (node* n: children){n->draw();}}
		void exit(){for (node* n: children){n->exit();}}
	};
}