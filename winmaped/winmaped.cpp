/*
	winmaped.cpp

	This is the place where all the non-OOP stuff is.  Just WinMain right now. :D

	And now, legal crap!

	This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages 
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and 
	redistribute it freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this 
	software in a product, an acknowledgment in the product documentation would be appreciated but is not required. 

	2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software. 

	3. This notice may not be removed or altered from any source distribution
*/

#define _WIN32_WINNT 0X0400     // I want Mousewheel stuff, dammit!
#include "winmaped.h"
#include "main.h"
    
// Globals ...? ... there are none! :D

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrev,LPSTR lpCmdline,int nCmdShow)
{
	Engine engine;
	engine.Execute(hThisInstance,nCmdShow);
	return 0;
}
