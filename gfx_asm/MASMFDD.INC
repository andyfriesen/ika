; MasmFDD.inc  (c) Mike Bibby  14/10/98

; Feel free to use or distribute it, part or whole, for any non-commercial use, 
; preferably crediting me.

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
     	dwSize					DWORD           ? 
     	dwsurfFlags        		DWORD           ? ; surf addedd
      	dwHeight     			DWORD       	? 
      	dwWidth   				DWORD       	? 
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
    	ddpfPixelFormat 		DDPIXELFORMAT 	<?>
     	ddssurfCaps 			DDSCAPS  		<?> ; surf added 
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


DDSCL_FULLSCREEN 		=	00000001h
DDSCL_ALLOWREBOOT       =	00000002h
DDSCL_NOWINDOWCHANGES 	= 	00000004h
DDSCL_NORMAL            =	00000008h
DDSCL_EXCLUSIVE 		=	00000010h
DDSCL_ALLOWMODEX    	=  	00000040h

 DDSD_CAPS       		= 	00000001h     
 DDSD_BACKBUFFERCOUNT  	=  	00000020h

 DDSCAPS_PRIMARYSURFACE = 	00000200h
 DDSCAPS_FLIP         	=  	00000010h
 DDSCAPS_FRONTBUFFER    =  	00000020h
 DDSCAPS_BACKBUFFER    	=  	00000004h
 DDSCAPS_COMPLEX       	=  	00000008h
 DDSCAPS_FLIP    		=  	00000010h
 DDSCAPS_VIDEOMEMORY	=	00004000h

 DDBLT_WAIT         	= 	01000000h
 DDBLT_COLORFILL    	=  	00000400h


;#########################################################

; equates for virtual table ...
DDRELEASE                       equ			8
DDBLT                           equ  		20
DDCREATESURFACE                 equ			24
DDFLIP                          equ			44
DDGETATTACHEDSURFACE            equ			48
DDGETDC                         equ			68
DDSETCOOPERATIVELEVEL           equ			80
DDSETDISPLAYMODE                equ			84
DDLOCK                          equ			100
DDRELEASEDC                     equ			104
DDUNLOCK                        equ			128

;################################################################################
;#                           Direct Draw prototypes                             #
;################################################################################

 DirectDrawCreate PROTO STDCALL :DWORD, :LPDIRECTDRAW, :DWORD

