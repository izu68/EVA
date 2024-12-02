#include "VGMPlay_Intf.h"

int main (void)
{
	VGMPlay_Init ();
	VGMPlay_Init2 ();
	OpenVGMFile ("./test.vgm");
		PlayVGM ();
	while (1)
	{
	
	};
	return 0;
}
