/** 
 * U Forbid
 *
 * @author		ÉòìÏ	<chrisniael@vip.qq.com>		<http://ÉòìÏ.cn>
 * @license		MIT License
 */

#include <iostream>
#include "core.h"

int main(int argc, const char *argv[])
{
	if(enable())
	{
		restart_desktop();
		std::cout << "Enable disk success!" << std::endl;
		std::cout << "Press any key to exit...";
		getchar();
	}
	else
	{
		std::cout << "Enable disk fail!" << std::endl;
		std::cout << "Press any key to exit...";
		getchar();
	}

	return 0;
}
