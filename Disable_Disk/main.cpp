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
	if(disable())
	{
		restart_desktop();
		std::cout << "Disable disk success!" << std::endl;
		std::cout << "Press any key to exit...";
		getchar();
	}
	else
	{
		std::cout << "Disable disk fail!" << std::endl;
		std::cout << "Press any key to exit...";
		getchar();
	}

	return 0;
}
