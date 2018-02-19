#include <iostream>

#include <base2.0/graphics/graphics.hpp>


int main(int argc, char const *argv[])
{
	std::cout << "Hello 7DRL!\n";

	window win(2500, 1600, "7DRL");

	// Window initialization
	{
		win.setBackgroundColor(100, 100, 100, 100);
		// shouldClose manages some state
		win.shouldClose();
		win.update();
		win.shouldClear(false);
		win.getBase()->setVerticalSyncEnabled(false);
		win.getBase()->setFramerateLimit(0);
	}
	
	while (!win.shouldClose())
	{
		win.update();
	}

	return 0;
}