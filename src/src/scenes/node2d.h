#pragma once
#include <string>
#include "../math/vec.h"
#include "../graph/texture.h"
#include "nodes.h"

namespace enginend {
	namespace nodes {
		struct node2d :public node {
			vec2 pos;
			node2d(){}
			node2d(float x,float y):pos(vec2{x,y}){}
		};
		struct rect :virtual public node2d{
			vec2 size;
			rect(){}
			rect(float x,float y,float w,float h):size(vec2{w,h}){
				this->pos=vec2{x,y};
			}
		};
		struct textured :virtual public rect{
			texture* tex;
			textured(){}
			textured(texture* tex,float x,float y,float w,float h):tex(tex){
				this->pos=vec2{x,y};this->size=vec2{w,h};
			}
			void boot()override{}
			void tick()override{}
			void draw()override{if(texture!=nullptr)DrawTexture(*texture,pos.x,pos.y,WHITE);}
			void exit()override{delete tex;}
		};
		struct animated : virtual public textured {
			Image animimage;
			int frames;
			int currentframe;
			int framedelay;
			int framecounter;
			unsigned int nextframeoffset;
			int prevframe;

			animated() : frames(0), currentframe(0), framedelay(6), framecounter(0), nextframeoffset(0) {
				animimage.data = nullptr;
			}

			animated(const char* gifpath, vec2 position, vec2 size, int delay = 6)
				: textured(nullptr, position.x, position.y, size.x, size.y),
				  framedelay(delay), currentframe(0), framecounter(0), frames(0), nextframeoffset(0)
			{
				animimage = LoadImageAnim(gifpath, &frames);
				if (frames > 0) {
					tex= new texture(LoadTextureFromImage(animimage));
				}
			}

			void tick() override {
				textured::tick();
				if (frames <= 1) return;

				framecounter++;
				if (framecounter >= framedelay) {
					framecounter = 0;
					currentframe++;
					if (currentframe >= frames) currentframe = 0;
					nextframeoffset = animimage.width * animimage.height * 4 * currentframe;
				}
			}
			void draw() override {
				
				if (prevframe!=currentframe){
					prevframe=currentframe;
					UpdateTexture(*this->tex,((unsigned char*)animimage.data)+nextframeoffset);
				}
				textured::draw();
			}
			void exit() override {
				if (animimage.data) UnloadImage(animimage);
				if (texture) {
					UnloadTexture(*texture);
					delete tex;
					tex= nullptr;
				}
			}
		};
		struct colored :virtual public rect{
			Color c;
			colored(){}
			colored(Color color,float x,float y,float w,float h):c(color){
				this->pos=vec2{x,y};this->size=vec2{w,h};
			}
			void boot()override{}
			void tick()override{}
			void draw()override{DrawRectangle(pos.x,pos.y,size.x,size.y,c);}
			void exit()override{}
		};
		struct tinted :virtual public colored,virtual public textured{
			tinted(){}
			tinted(texture* texture,Color color,float x,float y,float w,float h):
				node2d(x,y),
				rect(x,y,w,h),
				colored(color,x,y,w,h),
				textured(texture,x,y,w,h)
			{}
			void boot()override{this->colored::boot();this->texd::boot();}
			void tick()override{this->colored::tick();this->texd::tick();}
			void draw()override{if(texture!=nullptr)DrawTexture(*texture,pos.x,pos.y,c);}
			void exit()override{this->colored::exit();this->texd::exit();}
		};
		struct text :public tinted {
		protected:
			std::string result;
		public:
			Font font;
			float fs;
			Color txc;
			std::string content;
			text(){fs=20;}
			text(texture* texture,Color txcol,Color color,float x,float y,float w,float h,Font f,float fsize,std::string txt):
				font(f),fs(fsize),content(txt)
			{
				this->pos=vec2{x,y};this->size=vec2{w,h};this->tex=tex;this->c=color;this->txc=txcol;
			
				result=content;
				size_t initp=0;
				while((initp=result.find("\n",initp))!=std::string::npos){
					result.replace(initp,1,"\\n");
					initp+=2;
				}
			}
			void boot()override{this->tinted::boot();}
			void tick()override {
				this->tinted::tick();
				if(result!=content){
					result=content;
					size_t initp=0;
					while((initp=result.find("\n",initp))!=std::string::npos){
						result.replace(initp,1,"\\n");
						initp+=2;
					}
				}
			}
			void draw()override {
				vec2 minsize=MeasureTextEx(font,content.c_str(),fs,1);
				vec2 charsize=MeasureTextEx(font," ",fs,1);
				float p=charsize.x>charsize.y?charsize.x/minsize.x:charsize.y/minsize.y;
				p=p*2;
				int minh=(minsize.y>size.y)?minsize.y:size.y;
				int minw=(minsize.x>size.x)?minsize.x:size.x;
				DrawRectangle(pos.x-charsize.x,pos.y-charsize.y,minw+p,minh+p,c);
				DrawTextEx(font,content.c_str(),pos,fs,1,txc);
			}
			void exit()override{this->tinted::exit();}
		};
		struct button :virtual public tinted{
			std::function<void()> func;
			bool pressed;
			bool hover;
			button():pressed(false){}
			button(texture* texture,Color color,float x,float y,float w,float h,std::function<void()> f):func(f),pressed(false){
				this->pos=vec2{x,y};this->size=vec2{w,h};this->tex=tex;this->c=color;
			}
			void boot()override{this->tinted::boot();}
			void tick()override{
				this->tinted::tick();
				vec2 mouse=GetMousePosition();
				if(CheckCollisionPointRec(mouse,{pos.x,pos.y,size.x,size.y})){hover=true;
					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
						pressed=true;
						if(func)func();
					}else{
						pressed=false;
					}
				}else{
					hover=false;
				}
			}
			void draw()override {
				if(this->tex!=nullptr)DrawTexture(*texture,pos.x,pos.y,c);
				else DrawRectangle(pos.x,pos.y,size.x,size.y,c);
			}
			void exit()override{this->tinted::exit();}
		};
		struct labeledbutton :virtual public button {
			std::string label;
			Font font;
			int fs;
			Color txc;
			labeledbutton(std::string name,texture* texture,Color color,Color text,
				float x,float y,float w,float h,std::function<void()> f,
				Font fnt,int size):font(fnt),fs(size),txc(text){
				this->pos=vec2{x,y};this->size=vec2{w,h};this->tex=tex;this->c=color;
				this->func=f;this->pressed=false;
				this->label=name;
			}
			void boot()override{this->button::boot();}
			void tick()override{this->button::tick();}
			void draw()override{
				this->button::draw();
				vec2 tsize=MeasureTextEx(font,label.c_str(),fs,1);
				vec2 tpos={
					pos.x+(size.x-tsize.x)/2,
					pos.y+(size.y-tsize.y)/2
				};
				DrawTextEx(font,label.c_str(),tpos,fs,1,txc);
			}
			void exit()override{this->button::exit();}
		};
		struct slider :virtual public tinted{
			float val;
			float minv;
			float maxv;
			slider():val(0),minv(0),maxv(1){}
			slider(texture* texture,Color color,float x,float y,float w,float h,float min,float max,float v):val(v),minv(min),maxv(max){
				this->pos=vec2{x,y};this->size=vec2{x,y};this->tex=tex;this->c=color;
			}
			void boot()override{this->tinted::boot();}
			void tick()override{
				this->tinted::tick();
				vec2 mouse=GetMousePosition();
				if(CheckCollisionPointRec(mouse,{pos.x,pos.y,size.x,size.y})&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
					float t=(mouse.x-pos.x)/size.x;
					val=minv+t*(maxv-minv);
					if(val<minv)val=minv;
					if(val>maxv)val=maxv;
				}
			}
			void draw()override{
				DrawRectangle(pos.x,pos.y,size.x,size.y,DARKGRAY);
				float t=(val-minv)/(maxv-minv);
				DrawRectangle(pos.x,pos.y,size.x*t,size.y,c);
			}
			void exit()override{this->tinted::exit();}
		};
		struct textfield :public text{
			textfield(){}
			textfield(texture* texture,Color txcol,Color color,float x,float y,float w,float h,Font f,float fsize,std::string txt):
				text(texture,txcol,color,x,y,w,h,f,fsize,txt){}
			void boot()override{this->text::boot();}
			void tick()override{this->text::tick();}
			void draw()override{
				vec2 p=pos;
				vec2 charsize=MeasureTextEx(font," ",fs,0);
				vec2 minsize=MeasureTextEx(font,content.c_str(),fs,charsize.x/2);
				float po=charsize.x>charsize.y?charsize.x/charsize.y:charsize.y/charsize.x;po=po*5;
				int minh=(minsize.y>size.y)?minsize.y:size.y;
				int minw=(minsize.x>size.x)?minsize.x:size.x;
				DrawRectangle(pos.x-(po/2),pos.y-(po/2),minw+(po*1.1),minh+(po*1.1),c);
				DrawTextEx(font,content.c_str(),p,fs,charsize.x/2,this->txc);
			}
			void exit()override{this->text::exit();}
		};
		struct textinput :public text{
			bool active;
			int cpos;
			textinput():active(false),cpos(0){}
			textinput(texture* texture,Color txcol,Color color,float x,float y,float w,float h,Font f,float fsize):active(false),cpos(0){
				this->pos=vec2{x,y};this->size=vec2{x,y};this->tex=tex;this->c=color;this->font=f;this->content="";
				this->txc=txcol;this->fs=fsize;
			}
			void boot()override{this->text::boot();}
			void tick()override{
				this->text::tick();
				vec2 mouse=GetMousePosition();
				if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
					active=CheckCollisionPointRec(mouse,{pos.x,pos.y,size.x,size.y});
				}
				if(active){
					int key=GetCharPressed();
					while(key>0){
						if(key>=32&&key<=125){
							content+=static_cast<char>(key);
							cpos++;
						}
						key=GetCharPressed();
					}
					if(IsKeyPressed(KEY_BACKSPACE)&&content.length()>0){
						content.pop_back();
						cpos--;
					}
				}
			}
			void draw()override{
				this->text::draw();
				if(active)DrawRectangle(pos.x+MeasureText(content.c_str(),fs),pos.y,2,fs,{0,0,0,127});
			}
			void exit()override{this->text::exit();}
		};
		struct textinputfield :public textfield{
			bool active;
			int cpos;
			textinputfield():active(false),cpos(0){}
			textinputfield(texture* texture,Color txcol,Color color,float x,float y,float w,float h,Font f,float fsize):active(false),cpos(0),
				textfield(texture,txcol,color,x,y,w,h,f,fsize,""){}
			void boot()override{this->textfield::boot();}
			void tick()override{
				this->textfield::tick();
				vec2 mouse=GetMousePosition();
				if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
					active=CheckCollisionPointRec(mouse,{pos.x,pos.y,size.x,size.y});
				}
				if(active){
					int key=GetCharPressed();
					while(key>0){
						if(key>=32&&key<=125){
							content+=static_cast<char>(key);
							cpos++;
						}
						key=GetCharPressed();
					}
					if(IsKeyPressed(KEY_BACKSPACE)&&content.length()>0){
						content.pop_back();
						cpos--;
					}
					if(IsKeyPressed(KEY_ENTER)){
						content+='\n';
						cpos++;
					}
				}
			}
			void draw()override{
				this->textfield::draw();
				if(active){
					vec2 p=pos;
					float lh=fs+2;
					std::string line="";
					for(char ch:content){
						if(ch=='\n'){
							p.y+=lh;
							line="";
						}else{
							line+=ch;
						}
					}
					DrawRectangle(p.x+MeasureText(line.c_str(),fs),p.y,2,fs,BLACK);
				}
			}
			void exit()override{this->textfield::exit();}
		};
	}
}