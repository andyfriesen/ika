; MasmFDD.inc  (c) Mike Bibby  14/10/98

; Feel free to use or distribute it, part or whole, for any non-commercial use, 
; preferably crediting me.

; Seriously hacked up by tSB

;------------------------------------------------------------------------------
; misc types
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; DDCOLORKEY structure
;------------------------------------------------------------------------------
 _DDCOLORKEY struct
    dwColorSpaceLowValue    DWORD ? 
    dwColorSpaceHighValue   DWORD ? 
_DDCOLORKEY ends

DDCOLORKEY typedef _DDCOLORKEY
;------------------------------------------------------------------------------
; DDSCAPS structure
;------------------------------------------------------------------------------
_DDSCAPS struct
   dwCaps  		DWORD ?  
_DDSCAPS ends

DDSCAPS typedef _DDSCAPS
LPDDSCAPS typedef PTR DDSCAPS

;------------------------------------------------------------------------------
; DDPIXELFORMAT structure
;------------------------------------------------------------------------------

_DDPIXELFORMAT struct
           dwSize                   DWORD       ?   
           dwpixFlags  			DWORD  	? 
           dwFour   			DWORD  	? 
    union
           dwRGBBitCount 		DWORD  	?     
           dwYUVBitCount 		DWORD  	?   
           dwZBufferBitDepth        DWORD       ?   
           dwAlphaBitDepth 		DWORD  	?   
    ends
    union
           dwRBitMask   		DWORD  	?  
           dwYBitMask   		DWORD  	?  
    ends
    union
           dwGBitMask  			DWORD  	? 
           dwUBitMask 			DWORD  	?   
  	ends
    union
           dwBBitMask 			DWORD  	? 
           dwVBitMask 			DWORD 	? 
   	ends
    union
    
           dwRGBAlphaBitMask 	DWORD  	? 
           dwYUVAlphaBitMask 	DWORD 	? 
	ends

_DDPIXELFORMAT  ends

DDPIXELFORMAT  typedef _DDPIXELFORMAT 

LPDDPIXELFORMAT typedef PTR DDPIXELFORMAT 
;------------------------------------------------------------------------------
; DDSURFACEDESC structure
;------------------------------------------------------------------------------

_DDSURFACEDESC struct 
        dwSize                          DWORD           ? 
        dwFlags                         DWORD           ? ; surf addedd
      	dwHeight     			DWORD       	? 
        dwWidth                         DWORD           ? 
       	lPitch           		DWORD           ? 
       	dwBackBufferCount   	DWORD   		? 

    Union 
      	dwMipMapCount  			DWORD         	?
       	dwsurfZBufferBitDepth 	DWORD 			? ; surf addedd
        dwsurfRefreshRate  		DWORD  			? ; surf addedd                                                            described)
   	ends

      	dwsurfAlphaBitDepth 	DWORD 			? ; surf added 
      	dwReserved 				DWORD 			?  
      	lpSurface 				LPVOID 			? 
     	ddckCKDestOverlay 		DDCOLORKEY 	   	<?> 
    	ddckCKDestBlt 			DDCOLORKEY 		<?> 
    	ddckCKSrcOverlay 		DDCOLORKEY 		<?> 
    	ddckCKSrcBlt 			DDCOLORKEY 		<?> 
        ddpfPixelFormat                 DDPIXELFORMAT           <?>
        ddsCaps                         DDSCAPS                 <?> ; surf added 
_DDSURFACEDESC ends

DDSURFACEDESC typedef _DDSURFACEDESC
LPDDSURFACEDESC typedef PTR DDSURFACEDESC

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; DDBLTFX structure
;------------------------------------------------------------------------------

 _DDBLTFX struct 
    	dwSize  DWORD  0    
    	dwDDFX  DWORD  0    
    	dwROP   DWORD  0    
    	dwDDROP DWORD  0    
  	dwRotationAngle DWORD        0
      dwZBufferOpCode DWORD        0
  	dwZBufferLow DWORD           0
   	dwZBufferHigh DWORD          0
  	dwZBufferBaseDest DWORD      0 
  	dwZDestConstBitDepth DWORD   0
 union
  	dwZDestConst DWORD 0
 	lpDDSZBufferDest LPDIRECTDRAWSURFACE ?
 ends
	dwZSrcConstBitDepth DWORD 0                                      
 union
 	dwZSrcConst DWORD 0
  	lpDDSZBufferSrc LPDIRECTDRAWSURFACE ?                                            
 ends
 	dwAlphaEdgeBlendBitDepth DWORD 0
 	dwAlphaEdgeBlend DWORD 0
   	dwBltFxReserved  DWORD 0
 	dwAlphaDestConstBitDepth DWORD 0                             
union
  	dwAlphaDestConst DWORD 0
 	lpDDSAlphaDest LPDIRECTDRAWSURFACE ?                                                        
ends
 	dwAlphaSrcConstBitDepth DWORD 0 
union
 	dwAlphaSrcConst  DWORD 0
 	lpDDSAlphaSrc  LPDIRECTDRAWSURFACE ?                                         
ends
union
 	dwFillColor  DWORD 0
 	dwFillDepth DWORD ?
  	lpDDSPattern LPDIRECTDRAWSURFACE ?       
ends
 ddckDestColorkey  DDCOLORKEY <>
  ddckSrcColorkey  DDCOLORKEY <>
_DDBLTFX ends

DDBLTFX typedef  _DDBLTFX
LPDDBLTFX typedef PTR DDBLTFX 

;################################################################################
;#                           Direct Draw equates                                #
;################################################################################
	DD_OK     			=  0h
	DDFLIP_WAIT     	=  00000001h
;================= SETCOOPERATIVELEVEL FLAGS =========================


DDSCL_FULLSCREEN        =       00000001h
DDSCL_ALLOWREBOOT       =	00000002h
DDSCL_NOWINDOWCHANGES 	= 	00000004h
DDSCL_NORMAL            =	00000008h
DDSCL_EXCLUSIVE         =       00000010h
DDSCL_ALLOWMODEX    	=  	00000040h

 DDSD_CAPS              =       00000001h
 DDSD_HEIGHT            =       00000002h
 DDSD_WIDTH             =       00000004h
 DDSD_PITCH             =       00000008h
 DDSD_BACKBUFFERCOUNT   =       00000020h

 DDSCAPS_PRIMARYSURFACE = 	00000200h
 DDSCAPS_FLIP         	=  	00000010h
 DDSCAPS_FRONTBUFFER    =  	00000020h
 DDSCAPS_BACKBUFFER    	=  	00000004h
 DDSCAPS_COMPLEX       	=  	00000008h
 DDSCAPS_FLIP           =       00000010h
 DDSCAPS_VIDEOMEMORY	=	00004000h
 DDSCAPS_OFFSCREENPLAIN =       00000040H

 DDBLT_WAIT         	= 	01000000h
 DDBLT_COLORFILL    	=  	00000400h


;#########################################################

; equates for virtual table ...
; IUnknown
IUNKNOWN_QUERYINTERFACE         equ                     0
IUNKNOWN_ADDREF                 equ                     4
IUNKNOWN_RELEASE                equ                     8

; IDirectDraw
IDIRECTDRAW_COMPACT             equ                     12
IDIRECTDRAW_CREATECLIPPER       equ                     16
IDIRECTDRAW_CREATEPALETTE       equ                     20
IDIRECTDRAW_CREATESURFACE       equ                     24
IDIRECTDRAW_DUPLICATESURFACE    equ                     28
IDIRECTDRAW_ENUMDISPLAYMODES    equ                     32
IDIRECTDRAW_ENUMSURFACES        equ                     36
IDIRECTDRAW_FLIPTOGDISURFACE    equ                     40
IDIRECTDRAW_GETCAPS             equ                     44
IDIRECTDRAW_GETDISPLAYMODE      equ                     48
IDIRECTDRAW_GETFOURCCCODES      equ                     52
IDIRECTDRAW_GETGDISURFACE       equ                     56
IDIRECTDRAW_GETMONITORFREQUENCY equ                     60
IDIRECTDRAW_GETSCANLINE         equ                     64
IDIRECTDRAW_GETVERTICALBLANKSTATUS      equ             68
IDIRECTDRAW_INITIALIZE          equ                     72
IDIRECTDRAW_RESTOREDISPLAYMODE  equ                     76
IDIRECTDRAW_SETCOOPERATIVELEVEL equ                     80
IDIRECTDRAW_SETDISPLAYMODE      equ                     84
IDIRECTDRAW_WAITFORVERTICALBLANK        equ             88
; DirectDraw2 extra(s)
IDIRECTDRAW_GETAVAILABLEVIDMEM  equ                     92

IDIRECTDRAWPALETTE_GETCAPS      equ                     12
IDIRECTDRAWPALETTE_GETENTRIES   equ                     16
IDIRECTDRAWPALETTE_INITIALIZE   equ                     20
IDIRECTDRAWPALETTE_SETENTRIES   equ                     24

IDIRECTDRAWCLIPPER_GETCLIPLIST  equ                     12
IDIRECTDRAWCLIPPER_GETHWND      equ                     16
IDIRECTDRAWCLIPPER_INITIALIZE   equ                     20
IDIRECTDRAWCLIPPER_ISCLIPLISTCHANGED    equ             24
IDIRECTDRAWCLIPPER_SETCLIPLIST  equ                     28
IDIRECTDRAWCLIPPER_SETHWND      equ                     32

IDIRECTDRAWSURFACE_ADDATTACHEDSURFACE         equ                     12
IDIRECTDRAWSURFACE_ADDOVERLAYDIRTYRECT        equ                     16
IDIRECTDRAWSURFACE_BLT                        equ                     20
IDIRECTDRAWSURFACE_BLTBATCH                   equ                     24
IDIRECTDRAWSURFACE_BLTFAST                    equ                     28
IDIRECTDRAWSURFACE_DELETEATTACHEDSURFACE      equ                     32
IDIRECTDRAWSURFACE_ENUMATTACHEDSURFACES       equ                     36
IDIRECTDRAWSURFACE_ENUMOVERLAYZORDERS         equ                     40
IDIRECTDRAWSURFACE_FLIP                       equ                     44
IDIRECTDRAWSURFACE_GETATTACHEDSURFACE         equ                     48
IDIRECTDRAWSURFACE_GETBLTSTATUS               equ                     52
IDIRECTDRAWSURFACE_GETCAPS                    equ                     56
IDIRECTDRAWSURFACE_GETCLIPPER                 equ                     60
IDIRECTDRAWSURFACE_GETCOLORKEY                equ                     64
IDIRECTDRAWSURFACE_GETDC                      equ                     68
IDIRECTDRAWSURFACE_GETFLIPSTATUS              equ                     72
IDIRECTDRAWSURFACE_GETOVERLAYPOSITION         equ                     86 
IDIRECTDRAWSURFACE_GETPALETTE                 equ                     90
IDIRECTDRAWSURFACE_GETPIXELFORMAT             equ                     94
IDIRECTDRAWSURFACE_GETSURFACEDESC             equ                     98
IDIRECTDRAWSURFACE_INITIALIZE                 equ                     102
IDIRECTDRAWSURFACE_ISLOST                     equ                     106
IDIRECTDRAWSURFACE_LOCK                       equ                     110
IDIRECTDRAWSURFACE_RELEASEDC                  equ                     114
IDIRECTDRAWSURFACE_RESTORE                    equ                     118
IDIRECTDRAWSURFACE_SETCLIPPER                 equ                     122
IDIRECTDRAWSURFACE_SETCOLORKEY                equ                     126
IDIRECTDRAWSURFACE_SETOVERLAYPOSITION         equ                     32
IDIRECTDRAWSURFACE_SETPALETTE                 equ                     32
IDIRECTDRAWSURFACE_UNLOCK                     equ                     32
IDIRECTDRAWSURFACE_UPDATEOVERLAY              equ                     32
IDIRECTDRAWSURFACE_UPDATEOVERLAYDISPLAY       equ                     32
IDIRECTDRAWSURFACE_UPDATEOVERLAYZORDER        equ                     32

DDBLT                           equ  		20
DDFLIP                          equ			44
DDGETATTACHEDSURFACE            equ			48
DDGETDC                         equ			68
DDLOCK                          equ			100
DDRELEASEDC                     equ			104
DDUNLOCK                        equ			128

;################################################################################
;#                           Direct Draw prototypes                             #
;################################################################################

 DirectDrawCreate PROTO STDCALL :DWORD, :LPDIRECTDRAW, :DWORD

