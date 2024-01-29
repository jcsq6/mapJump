#include "gl_object.h"

#include "run_game.h"

int main()
{
	gl_instance gl(target_width, target_height, "Map Jumper");
	return run_game(gl, get_levels("levels"));
}

#ifdef _WIN32
int WinMain()
{
	return main();
}
#endif