; MasmFlip.inc  (c) Mike Bibby  14/10/98

; Feel free to use or distribute it, part or whole, for any non-commercial use, 
; preferably crediting me.


;################################################################################
;#                    fundamental equates and typedefs                          #
;################################################################################

	NULL				=	0					; null pointer
	FALSE				=	0					; boolean false
	TRUE				=	1					; boolean true


	BOOL				typedef DWORD			; boolean 
	UINT				typedef	DWORD			; unsigned integer 
	INTEGER                typedef	DWORD			; integer
	LONGINT                typedef	DWORD			; long integer
	LPARAM                 typedef	DWORD			; long parameter
	WPARAM                 typedef	DWORD			; word parameter
	WNDPROC                typedef	DWORD			; window procedure


      HWND                  typedef    DWORD            ; window handle
      HDC                   typedef    DWORD            ; device context handle
      HINSTANCE             typedef    DWORD            ; instance handle
      HCURSOR               typedef    DWORD            ; cursor handle
      HMENU                 typedef    DWORD            ; menu handle
      HICON                 typedef    DWORD            ; icon handle
      HBRUSH                typedef    DWORD            ; brush handle
  
      COLORREF              typedef    DWORD            ; rgb
	
	
	LPMSG				typedef	DWORD			; long pointer to message
	LPCSTR				typedef DWORD			; long pointer to string
	LPVOID				typedef	DWORD			; long pointer to void
	LPPAINTSTRUCT		typedef PTR PAINTSTRUCT	; long pointer to paint structure
	LPRECT				typedef	PTR RECT		; long pointer to rectangle


	LPDIRECTDRAW  typedef PTR DWORD
	LPDIRECTDRAWSURFACE typedef PTR DWORD
        LPDIRECTDRAWCLIPPER typedef PTR DWORD

;################################################################################
;#                           structure definitions                              #
;################################################################################
;
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;
RECT struct
	rect_left			LONGINT	  0		; left
	rect_top			LONGINT	  0		; top
	rect_right			LONGINT	  0		; right
	rect_bottom			LONGINT	  0		; bottom 
RECT ends

;
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;
WNDCLASS struct
    wc_style                    UINT        0        ; class style
    wc_lpfnWndProc              WNDPROC     0        ; window procedure
    wc_cbClsExtra               INTEGER     0        ; class extra data
    wc_cbWndExtra               INTEGER     0        ; window extra data
    wc_hInstance                HINSTANCE   0        ; class owner
    wc_hIcon                    HICON       0        ; icon handle
    wc_hCursor                  HCURSOR     0        ; cursor handle
    wc_hbrBackground            HBRUSH      0        ; background color
    wc_lpszMenuName             LPCSTR      0        ; menu name
    wc_lpszClassName            LPCSTR      0        ; class name
WNDCLASS ends

;
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;
MSG	struct
	msg_hwnd		HWND	  0		; window handle
	msg_message		UINT	  0		; message id
	msg_wParam		WPARAM	  0		; additional parameter
	msg_lParam		LPARAM	  0		; additional parameter
	msg_time		DWORD	  0		; time stamp
	msg_x			LONGINT	  0		; x coordinate
	msg_y			LONGINT	  0		; y coordinate
MSG	ends

;
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;
PAINTSTRUCT	struct
	ps_hdc			HDC	  	0		  	; device context handle
	ps_fErase		BOOL	FALSE		; background erase flag
	ps_leftPaint	LONGINT	0			; left of clipping rect
	ps_topPaint		LONGINT	0			; top of clipping rect
	ps_rightPaint	LONGINT	0		 	; right of clipping rect
	ps_bottomPaint	LONGINT	0		   	; bottom of clipping rect
	ps_fRestore		BOOL	FALSE		; reserved
	ps_fIncUpdate	BOOL	FALSE		; reserved
	ps_rgbReserved	BYTE	32 dup (0)	; reserved
PAINTSTRUCT	ends
;------------------------------------------------------------------------------
;
;RECT			struct
;rc_left			LONGINT	  0		;left margin
;rc_top			LONGINT	  0		;top margin
;rc_right		LONGINT	  0		;right margin
;rc_bottom		LONGINT	  0		;bottom margin
;RECT			ends
;RECT_			equ	  4+4+4+4
;
;------------------------------------------------------------------------------


;################################################################################
;#                            windows equates                                   #
;################################################################################

;===== window messages equates
	WM_ACTIVATEAPP      =  	001ch
	WM_PAINT			=	000fh
	WM_DESTROY			=	0002h	
	WM_CLOSE			=	0010h
	WM_KEYDOWN			=	0100h

	VK_ESCAPE			=	1bh

;============== equates used in various APIs ===================================

; used with ShowWindow
	SW_SHOWNORMAL		= 01h

; DrawText() options
	DT_CENTER       	= 01h
	DT_VCENTER      	= 04h
	DT_SINGLELINE   	= 20h


; cursor and icon IDs - defaults for LoadIcon and LoadCursor
	IDI_APPLICATION 	= 32512
	IDC_ARROW       	= 32512

;=============== equates used in defining and registering window class =========
	WHITE_BRUSH			=	 0

; Window class style options
	CS_VREDRAW         	= 0001h
	CS_HREDRAW         	= 0002h

;=== equates used in CreateWindowEx to create a window of a registered class ===
	WS_OVERLAPPED   = 00000000h
	WS_CAPTION      = WS_DLGFRAME + WS_BORDER
	WS_DLGFRAME		= 00400000h
	WS_BORDER		= 00800000h
	WS_SYSMENU      = 00080000h
	WS_THICKFRAME	= 00040000h
	WS_MINIMIZEBOX  = 00020000h
	WS_MAXIMIZEBOX  = 00010000h
	WS_POPUP		= 80000000h
	WS_OVERLAPPEDWINDOW = WS_OVERLAPPED + WS_CAPTION \
						+ WS_SYSMENU + WS_THICKFRAME \
						+ WS_MINIMIZEBOX + WS_MAXIMIZEBOX
						
	WS_EX_TOPMOST	= 00000008h
	
;================== equates used in PeekMessage =============						
	PM_NOREMOVE		=	00h
	PM_REMOVE		=	01h
	PM_NOYIELD		=	02h
;================== equates used TextOutA =============
	TA_LEFT			=	0000h
	TA_RIGHT		=	0002h
	TA_CENTER		=	0006h
	TA_TOP			=	000h
	TA_BOTTOM		=	008h
	TA_BASELINE		=	018h
;================== equates used in MessageBox =============
	MB_OK			=	000000000h

;################################################################################
;#                               prototypes                                     #
;################################################################################


	wsprintfA  	PROTO	C		;alternatively EXTERN _wsprintfA :PROC
	lstrlenA	PROTO	STDCALL	:LPCSTR 
	
;################################################################################
;#                           prototypes of APIs                                 #
;################################################################################


;--- used in WinMain 	---------------------------
	GetModuleHandleA	PROTO	STDCALL	:DWORD

	ShowWindow			PROTO	STDCALL	:HWND, :SDWORD
	UpdateWindow		PROTO	STDCALL	:HWND

; these are used in the message loop ----------
	PeekMessageA		PROTO	STDCALL	:LPMSG, :HWND, :UINT, :UINT, :UINT
	GetMessageA			PROTO	STDCALL	:LPMSG, :HWND, :UINT, :UINT
	TranslateMessage	PROTO	STDCALL	:LPMSG
	DispatchMessageA	PROTO	STDCALL	:LPMSG
	WaitMessage			PROTO	STDCALL
	ExitProcess			PROTO	STDCALL	:DWORD
	PostMessageA		PROTO	STDCALL	:HWND, :UINT, :WPARAM, :LPARAM
	
;---these are used in WindowProc ----------------

	; used if a message isn't handled specifically in the window routine

	DefWindowProcA		PROTO	STDCALL	:HWND, :UINT, :WPARAM, :LPARAM

	; used to handle WM_PAINT messages

	BeginPaint			PROTO	STDCALL	:HWND, 	:LPPAINTSTRUCT
	GetClientRect		PROTO	STDCALL	:HWND, 	:LPRECT
	DrawTextA			PROTO	STDCALL	:HDC, 	:LPCSTR, :UINT, :LPRECT, :UINT
	EndPaint			PROTO	STDCALL	:HWND, 	:LPPAINTSTRUCT

	; used to handle WM_DESTROY messages

	PostQuitMessage		PROTO	STDCALL	:SWORD

;--- these are used in Initialise ---------------

	LoadCursorA			PROTO	STDCALL	:HINSTANCE, :LPCSTR
	LoadIconA			PROTO	STDCALL	:HINSTANCE, :LPCSTR
	GetStockObject		PROTO	STDCALL	:SDWORD				;used to get a brush

	RegisterClassA	    PROTO STDCALL :DWORD

	CreateWindowExA		PROTO	STDCALL	:DWORD, :LPCSTR, :LPCSTR, :DWORD, :SDWORD,\
	 					:SDWORD, :SDWORD, :SDWORD, :HWND, :HMENU, :HINSTANCE, :DWORD
	 					
;--- these are used in Update -------------------
	GetDC				PROTO	STDCALL	:HWND
	TextOutA			PROTO	STDCALL	:HDC, 	:DWORD, :DWORD, :LPCSTR, :DWORD
	ReleaseDC			PROTO	STDCALL	:HWND, 	:HDC	
	
	SetTextAlign		PROTO	STDCALL	:HDC, 	:UINT
	SetTextColor		PROTO	STDCALL	:HDC, 	:COLORREF
	MessageBoxA			PROTO	STDCALL	:HWND, 	:LPCSTR, :LPCSTR, :UINT
	
;-- this is used in GetFrameRate ----------------
	timeGetTime 		PROTO 	STDCALL
