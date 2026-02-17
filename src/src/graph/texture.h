#include <string>
class texture{
public:
	std::string path;
	int id,width,height,mode;
	texture(std::string);
	void load();
	void draw();
	void unload();
	~texture();
};