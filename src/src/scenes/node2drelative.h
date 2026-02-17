#pragma once
#include <string>
#include "../graph/texture.h"
#include "../math/vec.h"
#include "nodes.h"

namespace enginend{
	namespace nodes{
		namespace relative {
			struct node2d :public node {
				double x;
				double y;
				double w;
				double h;
				node2d(){x=0;y=0;w=0;h=0;}
				node2d(double x,double y,double w=0,double h=0):x(x),y(y),w(w),h(h){}
			};
			struct rect :virtual public node2d{
				rect(){}
				rect(double x,double y,double w,double h):node2d(x,y,w,h){}
			};
			struct textured :virtual public rect{
				texture* tex;
				textured(){texture=nullptr;}
				textured(texture* texture,double x,double y,double w,double h):texture(texture),rect(x,y,w,h){}
				void boot() override{}
				void tick() override{}
				void draw()override{
					if(texture==nullptr)return;
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					tiny::echo("og: %f %f %f %f", x,y,w,h);
					tiny::echo("transformed: %f %f %f %f", ax, ay, aw, ah);
					DrawTexturePro(*texture,{0,0,(float)texture->width,(float)texture->height},{ax,ay,aw,ah},{0,0},0,WHITE);
				}
				void exit()override {
					if(texture){
						UnloadTexture(*texture);
						delete tex;
						texture=nullptr;
					}
				}
			};
			struct animated :virtual public textured{
				Image animimage;
				int frames;
				int currentframe;
				int framedelay;
				int framecounter;
				int prevframe;
				unsigned int nextframeoffset;
				animated():frames(0),currentframe(1),framedelay(6),framecounter(0),nextframeoffset(0){
					animimage.data=nullptr;
					prevframe=currentframe;
				}
				animated(const char* gifpath,double x,double y,double w,double h,int delay=6):
					textured(nullptr,x,y,w,h),framedelay(delay),currentframe(1),framecounter(0),frames(0),nextframeoffset(0)
				{
					prevframe=currentframe;
					this->x=x; this->y=y; this->w=w; this->h=h;
					animimage=LoadImageAnim(gifpath,&frames);
					if(frames>0){
						texture=new texture(LoadTextureFromImage(animimage));
					}
				}
				void tick()override{
					if(frames<=1)return;
					framecounter++;
					if(framecounter>=framedelay){
						framecounter=0;
						currentframe++;
						if(currentframe>=frames)currentframe=0;
						nextframeoffset=animimage.width*animimage.height*4*currentframe;
						tiny::echo("updating node\nframes: %i\n current frame: %i",frames,currentframe);
						tiny::echo("%i",nextframeoffset);
					}
				}
				void draw()override {
					if (prevframe!=currentframe){
						prevframe=currentframe;
						UpdateTexture(*this->tex,((unsigned char*)animimage.data)+nextframeoffset);
					}
					textured::draw();
				}
				void exit()override{
					if(animimage.data)UnloadImage(animimage);
					textured::exit();
				}
			};
			struct colored :virtual public rect{
				Color c;
				colored(){}
				colored(Color color,double x,double y,double w,double h):c(color),rect(x,y,w,h){}
				void boot()override{}
				void tick()override{}
				void draw()override{
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					DrawRectangle(ax,ay,aw,ah,c);
				}
				void exit()override{}
			};
			struct tinted :virtual public colored,virtual public textured{
				tinted(){}
				tinted(texture* texture,Color color,double x,double y,double w,double h):
					node2d(x,y,w,h),
					rect(x,y,w,h),
					colored(color,x,y,w,h),
					textured(texture,x,y,w,h)
				{}
				void boot()override{}
				void tick()override{}
				void draw()override{
					if(texture==nullptr)return;
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					DrawTexturePro(*texture,{0,0,(float)texture->width,(float)texture->height},{ax,ay,aw,ah},{0,0},0,c);
				}
				void exit()override{
					textured::exit();
				}
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
				text(texture* texture,Color txcol,Color color,double x,double y,double w,double h,Font f,float fsize,std::string txt):
					//tinted(texture,color,x,y,w,h),
					font(f),fs(fsize),content(txt),txc(txcol)
				{
					this->x=x;this->y=y;this->w=w;this->h=h;
					this->tex=tex;this->c=color;
					result=content;
					size_t initp=0;
					while((initp=result.find("\n",initp))!=std::string::npos){
						result.replace(initp,1,"\\n");
						initp+=2;
					}
				}
				void boot()override{}
				void tick()override {
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
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					
					float aw=w*sw;
					float ah=h*sh;
					tiny::echo("og: %f %f %f %f", x,y,w,h);
					tiny::echo("drawing text: %s", content.c_str());
					tiny::echo("transformed: %f %f %f %f", ax, ay, aw, ah);
					vec2 minsize=MeasureTextEx(font,content.c_str(),fs,1);
					vec2 charsize=MeasureTextEx(font," ",fs,1);
					float p=charsize.x>charsize.y?charsize.x/minsize.x:charsize.y/minsize.y;
					p=p*2;
					int minh=(minsize.y>ah)?minsize.y:ah;
					int minw=(minsize.x>aw)?minsize.x:aw;
					DrawRectangle(ax-charsize.x,ay-charsize.y,minw+p,minh+p,c);
					DrawTextEx(font,content.c_str(),{ax,ay},fs,1,txc);
				}
				void exit()override{
					tinted::exit();
				}
			};
			struct button :virtual public tinted{
				void(*func)();
				bool pressed;
				bool hover;
				button():func(nullptr),pressed(false),hover(false){}
				button(texture* texture,Color color,double x,double y,double w,double h,void(*f)()):func(f),pressed(false),hover(false),tinted(texture,color,x,y,w,h){}
				void boot()override{}
				void tick()override{
					vec2 mouse=GetMousePosition();
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					Rectangle r={float(x*sw),float(y*sh),float(w*sw),float(h*sh)};
					if(CheckCollisionPointRec(mouse,r)){hover=true;
						if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
							pressed=true;
							if(func)func();
						}else{
							pressed=false;
						}
					}else{
						hover=false;
						pressed=false;
					}
				}
				void draw()override {
					tinted::draw();
				}
				void exit()override{
					tinted::exit();
				}
			};
			struct labeledbutton :virtual public button {
				std::string label;
				Font font;
				int fs;
				Color txc;
				labeledbutton(std::string name,texture* texture,Color color,Color text,
					double x,double y,double w,double h,void(*f)(),
					Font fnt,int size):font(fnt),fs(size),txc(text),label(name),
					button(texture,color,x,y,w,h,f)
				{}
				void boot()override{}
				void tick()override{
					button::tick();
				}
				void draw()override{
					button::draw();
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					vec2 tsize=MeasureTextEx(font,label.c_str(),fs,1);
					vec2 tpos={
						ax+(aw-tsize.x)/2,
						ay+(ah-tsize.y)/2
					 };
					DrawTextEx(font,label.c_str(),tpos,fs,1,txc);
				}
				void exit()override{
					button::exit();
				}
			};
			struct slider :virtual public tinted{
				float val;
				float minv;
				float maxv;
				slider():val(0),minv(0),maxv(1){}
				slider(texture* texture,Color color,double x,double y,double w,double h,float min,float max,float v):val(v),minv(min),maxv(max),tinted(texture,color,x,y,w,h){}
				void boot()override{}
				void tick()override{
					vec2 mouse=GetMousePosition();
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					Rectangle r={float(x*sw),float(y*sh),float(w*sw),float(h*sh)};
					if(CheckCollisionPointRec(mouse,r)&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
						float t=(mouse.x-(x*sw))/(w*sw);
						val=minv+t*(maxv-minv);
						if(val<minv)val=minv;
						if(val>maxv)val=maxv;
					}
				}
				void draw()override{
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					DrawRectangle(ax,ay,aw,ah,DARKGRAY);
					float t=(val-minv)/(maxv-minv);
					DrawRectangle(ax,ay,aw*t,ah,c);
				}
				void exit()override{
					tinted::exit();
				}
			};
			struct textfield :public text{
				textfield(){}
				textfield(texture* texture,Color txcol,Color color,double x,double y,double w,double h,Font f,float fsize,std::string txt):
					text(texture,txcol,color,x,y,w,h,f,fsize,txt){}
				void boot()override{}
				void tick()override{
					text::tick();
				}
				void draw()override{
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					float ax=x*sw;
					float ay=y*sh;
					float aw=w*sw;
					float ah=h*sh;
					vec2 charsize=MeasureTextEx(font," ",fs,0);
					vec2 minsize=MeasureTextEx(font,content.c_str(),fs,charsize.x/2);
					float po=charsize.x>charsize.y?charsize.x/charsize.y:charsize.y/charsize.x;po=po*5;
					int minh=(minsize.y>ah)?minsize.y:ah;
					int minw=(minsize.x>aw)?minsize.x:aw;
					DrawRectangle(ax-(po/2),ay-(po/2),minw+(po*1.1),minh+(po*1.1),c);
					DrawTextEx(font,content.c_str(),{ax,ay},fs,charsize.x/2,txc);
				}
				void exit()override{
					text::exit();
				}
			};
			struct textinput :public text{
				bool active;
				int cpos;
				textinput():active(false),cpos(0){}
				textinput(texture* texture,Color txcol,Color color,double x,double y,double w,double h,Font f,float fsize):active(false),cpos(0),
					text(texture,txcol,color,x,y,w,h,f,fsize,""){}
				void boot()override{}
				void tick()override{
					text::tick();
					vec2 mouse=GetMousePosition();
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					Rectangle r={float(x*sw),float(y*sh),float(w*sw),float(h*sh)};
					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
						active=CheckCollisionPointRec(mouse,r);
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
					text::draw();
					if(active){
						float sw=GetScreenWidth();
						float sh=GetScreenHeight();
						float ax=x*sw;
						float ay=y*sh;
						DrawRectangle(ax+MeasureTextEx(font,content.c_str(),fs,1).x,ay,2,fs,{0,0,0,127});
					}
				}
				void exit()override{
					text::exit();
				}
			};
			struct textinputfield :public textfield{
				bool active;
				int cpos;
				textinputfield():active(false),cpos(0){}
				textinputfield(texture* texture,Color txcol,Color color,double x,double y,double w,double h,Font f,float fsize):active(false),cpos(0),
					textfield(texture,txcol,color,x,y,w,h,f,fsize,""){}
				void boot()override{}
				void tick()override{
					textfield::tick();
					vec2 mouse=GetMousePosition();
					float sw=GetScreenWidth();
					float sh=GetScreenHeight();
					Rectangle r={float(x*sw),float(y*sh),float(w*sw),float(h*sh)};
					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
						active=CheckCollisionPointRec(mouse,r);
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
					textfield::draw();
					if(active){
						float sw=GetScreenWidth();
						float sh=GetScreenHeight();
						float ax=x*sw;
						float ay=y*sh;
						float lh=fs+2;
						vec2 p={ax,ay};
						std::string line="";
						for(char ch:content){
							if(ch=='\n'){
								p.y+=lh;
								line="";
							}else{
								line+=ch;
							}
						}
						DrawRectangle(p.x+MeasureTextEx(font,line.c_str(),fs,1).x,p.y,2,fs,BLACK);
					}
				}
				void exit()override{
					textfield::exit();
				}
			};
		}
	}
}