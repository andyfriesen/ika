/* 
  Keyboard/mouse handling stuff.
  This was originally used in v2.6, but I thought "what the hell", and copied it. ^_~

  This was originally meant to mimic aen's keyboard handler, but has since
  been reversed. While aen's handler "feeds" keys to processes, this one
  simply puts them in a queue for the process to grab on it's own.
  
	ChangeLog
	+ <tSB> 11.05.00 - Initial writing
	+ <tSB> 11.07.00 - DirectInput won't compile right.  Re-started, using Win32's messaging system to handle keypresses.
	+ <tSB> 11.07.00 - This doesn't work either :P Different DirectX libs work now. ^_^
	+ <tSB> 11.09.00 - woo, another overhaul.  Based on vecna's Winv1/Blackstar code.
	+ <tSB> 11.16.00 - Mouse code added
	+ <tSB> 12.05.00 - Mouse code rehashed, using DInput again.
	+ <tSB> 06.23.01 - Six months already? O_O  Lots and lots of tweakage.
	
*/

#define DIRECTINPUT_VERSION 0X0500
#include <dinput.h>
#include "input.h" // woo!  no dependencies! :D
#include "log.h"

static byte key_ascii_tbl[128] =
{
	  0,   0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',   8,   9,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  13,   0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  39, '`',   0,  92, 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,   3,   3,   3,   3,   3,
	  3,   3,   3,   3,   3,   0,   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,
	  0,   0,   0, 127,   0,   0,  92,   3,   3,   0,   0,   0,   0,   0,   0,   0,
	 13,   0, '/',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   127,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '/',   0,   0,   0,   0,   0
};

static byte key_shift_tbl[128] =
{
      0,   0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',   8,   9,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',  13,   0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':','\"', '~',   0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?',   0, '*',   0,   1,   0,   1,   1,   1,   1,   1,
	  1,   1,   1,   1,   1,   0,   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,
	  0,   0,   1, 127,   0,   0,   0,   1,   1,   0,   0,   0,   0,   0,   0,   0,
	 13,   0, '/',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 127,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '/',   0,   0,   0,   0,   0
};

Input::Input() : 
	up(control[0]),
	down(control[1]),
	left(control[2]),
	right(control[3]),
	enter(control[4]),
	cancel(control[5])
{
	lpdi=NULL;
	keybd=NULL;
	mouse=NULL;

	for (int i=0; i<nControls; i++)
		control[i]=false;

	ZeroMemory(&keys,sizeof keys);

	BindKey(0,DIK_UP);
	BindKey(1,DIK_DOWN);
	BindKey(2,DIK_LEFT);
	BindKey(3,DIK_RIGHT);

	BindKey(4,DIK_RETURN);
	BindKey(5,DIK_ESCAPE);
}

Input::~Input()
{
	if (lpdi!=NULL)
		ShutDown();
}

inline int Input::Test(HRESULT result,char* errmsg)
// Just to keep the error checking code from taking up so much space in the init routine.
{
	if (result!=DI_OK)
	{
		log(errmsg);
		ShutDown();
		return 0;
	}
	return 1;
}

int Input::Init(HINSTANCE hinst,HWND hwnd)
{
	HRESULT result;
	DIPROPDWORD dipdw;
	
	hInst=hinst;
	hWnd=hwnd;
	
	result=DirectInputCreate(hinst,DIRECTINPUT_VERSION,&lpdi,NULL);
	if (!Test(result,"DI:DInputCreate")) return 0;

	// -------------keyboard initizlization------------
	result=lpdi->CreateDevice(GUID_SysKeyboard,&keybd,NULL);
	if (!Test(result,"DI:CreateDevice")) return 0;

	result=keybd->SetDataFormat(&c_dfDIKeyboard);
	if (!Test(result,"DI:SetDataFormat")) return 0;

	result=keybd->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (!Test(result,"DI:SetCo-oplevel")) return 0;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 128;
	result=keybd->SetProperty(DIPROP_BUFFERSIZE,&dipdw.diph);
	if (!Test(result,"DI:SetProperty")) return 0;

	keybd->Acquire();
	kb_start=kb_end=0;

	// ---------------mouse-----------------
	result=lpdi->CreateDevice(GUID_SysMouse,&mouse,NULL);
	if (!Test(result,"DI:CreateMouseDevice")) return 0;

	result=mouse->SetDataFormat(&c_dfDIMouse);
	if (!Test(result,"DI:SetMouseDataFormat")) return 0;

	result=mouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (!Test(result,"DI:SetMouseCoOpLevel")) return 0;

	mclip.top=mclip.left=0;
	mclip.right=320;
	mclip.bottom=200;

	return 1;
}

void Input::ShutDown()
{
	if (lpdi==NULL) return;
	if (keybd!=NULL)
	{
		keybd->Unacquire();
		keybd->Release();
		keybd=NULL;
	}
	if (mouse!=NULL)
	{
		mouse->Unacquire();
		mouse->Release();
		mouse=NULL;
	}
	// etc... for mouse/joystick/etc...
	lpdi->Release();
	lpdi=NULL;
}

void Input::Poll() // updates the key[] array.  This is called in winproc in response to WM_KEYDOWN and WM_KEYUP
{
	HRESULT result;
	DIDEVICEOBJECTDATA didata[128];
	DWORD numentries;
	
	numentries=128;
	if (!keybd) 
	{
		log("input::poll: No keyboard object!");
		return;
	}

	// read from the keyboard
	result=keybd->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),didata,&numentries,0);
	if (result!=DI_OK && result!=DI_BUFFEROVERFLOW)                                                    // HEY! D:<
	{
		keybd->Acquire();                                                               // re-acquire it
		result=keybd->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),didata,&numentries,0);   // and try again!
	}
	
	if (!numentries || result==DIERR_OTHERAPPHASPRIO) return; // TODO: joystick?
	
	for (int i=0; i<numentries; i++)
	{
		int k=didata[i].dwOfs;
		bool kdown=didata[i].dwData&0x80?true:false;
		
		// First off, DX has separate codes for the control keys, and alt keys, etc...
		// We don't want that.  Convert 'em.
		if (k==DIK_RCONTROL) k=DIK_LCONTROL;
		if (k==DIK_RMENU)    k=DIK_LMENU;
		
		keys[k].bPressed=kdown;
		last_pressed=k;									// this is kinda handy
		
		if (kdown && kb_end!=kb_start+1)				// if the key's down and the buffer has room
			key_buffer[kb_end++]=k;						// add it to the queue
		// TODO: key repeating?
		
		if (keys[k].bIsbound)
		{
			control[keys[k].nControl]=kdown;			// update the virtual control, if there is one.
			
			if (kdown)
				controlbuffer.push(keys[k].nControl);
		}		
	}
}

void Input::Update() // updates the direction variables and the virtual buttons (b1, b2, etc..)
{
	Poll();
	
	UpdateMouse();
}

void Input::BindKey(int nButtonidx,int nKeycode)
{
	log("BindKey %i to %i",nButtonidx,nKeycode);
	if (nKeycode<0 || nKeycode>255)
		return;
	if (nButtonidx<0 || nButtonidx>nControls)
		return;
	keys[nKeycode].nControl=nButtonidx;
	keys[nKeycode].bIsbound=true;
}

void Input::UnbindKey(int nKeycode)
{
	if (nKeycode<0 || nKeycode>255)
		return;
	keys[nKeycode].bIsbound=false;
}

int Input::GetKey()
// gets the next key from the buffer, or 0 if there isn't one
{
	if (kb_start==kb_end) return 0; // nope!  nuthin here
	
	return key_buffer[kb_start++];
}

void Input::StuffKey(int key)
{
	if (kb_end==kb_start+1)
		return;

	key_buffer[kb_end++]=key;
}

void Input::ClearKeys()
// clears the keyboard buffer (duh!)
{
	kb_end=kb_start=0;
}

int Input::NextControl()
{
//	if (ctl_start==ctl_end)	return -1;	// 0 is a valid control index. :(

//	return ctl_buffer[ctl_start++];
	if (controlbuffer.empty())
		return -1;
	else
	{
		int i=controlbuffer.front();
		controlbuffer.pop();

		return i;
	}
}

void Input::ClearControls()
{
//	ctl_start=ctl_end=0;
	while (!controlbuffer.empty())
		controlbuffer.pop();
}

char Input::Scan2ASCII(int scancode)
{
	if (!scancode)
		return 0;
	
	if (keys[DIK_LSHIFT].bPressed || keys[DIK_RSHIFT].bPressed)
		return key_shift_tbl[scancode];
	else
		return key_ascii_tbl[scancode];
}

void Input::MoveMouse(int x,int y)
{
	mousex=x; mousey=y;
}

void Input::UpdateMouse()
{
	DIMOUSESTATE dims;
	HRESULT result;

	if (!mouse) 
	{
		log("input::updatemouse: mouse object is null!");
		return;
	}
	mouse->Acquire();
	result=mouse->GetDeviceState(sizeof(dims),&dims);
	if (!Test(result,"DirectInput Error: Error reading the mouse")) return;
	
	mousex+=dims.lX;
	mousey+=dims.lY;
	
	if (mousex<mclip.left) mousex=mclip.left;
	if (mousex>mclip.right) mousex=mclip.right;
	if (mousey<mclip.top) mousey=mclip.top;
	if (mousey>mclip.bottom) mousey=mclip.bottom;
	
	mouseb=0;
	if (dims.rgbButtons[0]&0x80) mouseb|=1;
	if (dims.rgbButtons[1]&0x80) mouseb|=2;
	if (dims.rgbButtons[2]&0x80) mouseb|=4;
	if (dims.rgbButtons[3]&0x80) mouseb|=8;
}

void Input::ClipMouse(int x1,int y1,int x2,int y2)
{
	mclip.left=x1;
	mclip.right=x2;
	mclip.top=y1;
	mclip.bottom=y2;
}

void Input::HideMouse()
{
	HRESULT result=mouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		log("input.hidemouse failed");
}

void Input::ShowMouse()
{
	HRESULT result=mouse->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(result))
		log("input.showmouse failed");
}