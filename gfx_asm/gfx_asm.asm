;
;
;       gfx_asm.asm - The supafast(tm) ika graphics plugin
;
;       Goal: 32 bit driver.  Half alpha blending only. SUPAFAST FAST FAST!!!
;
;       by Andy Friesen (aka tSB)
;

        .586
        .model flat,stdcall
        option casemap:none     ; case sensitive

        include masmflip.inc
        include ddraw.inc

        includelib \masm32\lib\ddraw.lib

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        return MACRO arg
                mov     eax,arg
                ret
        ENDM

    m2m MACRO   a1,a2
        push    a1
        pop a2
    ENDM

    calldd  MACRO   ddobject,ddfunc
        mov eax,ddobject
        push    eax
        mov eax,[eax]
        call    DWORD PTR[eax+ddfunc]
    ENDM

    zeromem MACRO   dest,size
        LOCAL   blah

        mov ebx,OFFSET dest
        mov al,0
        mov ecx,size

        blah:
            mov [ebx],al
            add ebx,1
            dec ecx
        jnz blah
    ENDM
        

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Prototypes

    gfxShutdown PROTO   STDCALL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        HANDLE  typedef DWORD   ; Image handle

; Globals
.data
                ; DirectDraw objects and such
        lpdd            LPDIRECTDRAW            NULL
        lpMainsurf      LPDIRECTDRAWSURFACE     NULL
        lpBacksurf      LPDIRECTDRAWSURFACE     NULL
        lpClipper       LPDIRECTDRAWCLIPPER     NULL
        bltfx           DDBLTFX                 <0>
    ddsd        DDSURFACEDESC       <0>
    ddscaps     DDSCAPS         <0>

    hCurwnd     HWND            ?

                ; Pixel format information
        nRedsize        DWORD   0
        nBluesize       DWORD   0
        nGreensize      DWORD   0
        nAlphasize      DWORD   0
        nRedpos         DWORD   0
        nBluepos        DWORD   0
        nGreenpos       DWORD   0
        nAlphapos       DWORD   0

                ; Screen mode information
        nBytesperpixel  DWORD   0
        nXres           DWORD   0
        nYres           DWORD   0
        bFullscreen     BYTE    0
        hScreen         HANDLE  NULL
        hRenderdest     HANDLE  NULL
        maincliprect    RECT    <?>

        nImagecount     DWORD   0
        bInited         BYTE    FALSE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.code

LibMain proc    hInstDll:DWORD, reason:DWORD, unused:DWORD

        ; I... don't really need to do anything upon loading. :)

LibMain Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Returns the version of the graphics driver.
gfxGetVersion   proc

        return  1

gfxGetVersion   Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Inits the graphics mode
gfxInit         proc    STDCALL, hWnd:HWND,xres:DWORD,yres:DWORD,bpp:DWORD,bfullscreen:BYTE

        ; locals
    LOCAL   result  :   DWORD;

    .IF bInited!=0                      ; Is this being called redundantly?
        return  FALSE                   ; Yes.  Fail
    .ENDIF

    mov bInited,TRUE                    ; set the flag, so that we know it's been set up

    m2m nXres,xres
    m2m nYres,yres
    mov nBytesperpixel,32               ; ignore the passed value for now.  Maybe write a whole new driver for 16 bit.
    m2m hCurwnd,hWnd
    mov al,bfullscreen
    mov bFullscreen,al

    .IF lpdd==NULL
        INVOKE  DirectDrawCreate,NULL,ADDR lpdd,NULL
        .IF eax!=DD_OK
            return FALSE                ; Directdraw create failed :(
        .ENDIF
    .ENDIF

    .IF bfullscreen!=0

    ; Set the cooperative level
        push    (DDSCL_EXCLUSIVE or DDSCL_FULLSCREEN)
        push    hWnd
        calldd  lpdd,IDIRECTDRAW_SETCOOPERATIVELEVEL
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return FALSE
        .ENDIF

    ; Set the display mode
        push    32                  ; bits per pixel
        push    nYres
        push    nXres
        calldd  lpdd,IDIRECTDRAW_SETDISPLAYMODE
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return FALSE
        .ENDIF

    ; Create the primary surface
        mov     ddsd.dwSize,SIZEOF ddsd
        mov     ddsd.dwFlags,(DDSD_CAPS or DDSD_BACKBUFFERCOUNT)
        mov     ddsd.ddsCaps.dwCaps,(DDSCAPS_PRIMARYSURFACE or DDSCAPS_COMPLEX or DDSCAPS_FLIP)
        mov     ddsd.dwBackBufferCount,1
        push    NULL
        push    OFFSET lpMainsurf
        push    OFFSET ddsd
        calldd  lpdd,IDIRECTDRAW_CREATESURFACE
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return FALSE
        .ENDIF

    ; Grab the back buffer (which is attached to the primary)
        mov ddscaps.dwCaps,DDSCAPS_BACKBUFFER
        push    OFFSET lpBacksurf
        push    OFFSET ddscaps
        calldd  lpMainsurf,IDIRECTDRAWSURFACE_GETATTACHEDSURFACE
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return FALSE
        .ENDIF
    .ELSE                           ; If windowed mode

    ; Set the cooperative level
        push    DDSCL_NORMAL
        push    hWnd
        calldd  lpdd,IDIRECTDRAW_SETCOOPERATIVELEVEL
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF

    ; Create the primary surface
        mov     ddsd.dwSize,SIZEOF ddsd
        mov     ddsd.dwFlags,DDSD_CAPS;
        mov     ddsd.ddsCaps.dwCaps,DDSCAPS_PRIMARYSURFACE;
        push    NULL
        push    OFFSET  lpMainsurf
        push    OFFSET  ddsd
        calldd  lpdd,IDIRECTDRAW_CREATESURFACE
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF

    ; Create the main clipper
        push    NULL
        push    OFFSET  lpClipper
        push    0
        calldd  lpdd,IDIRECTDRAW_CREATECLIPPER
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF

    ; Set the clipper to clip to the client window
        push    hWnd
        push    0
        calldd  lpClipper,IDIRECTDRAWCLIPPER_SETHWND
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF

    ; Set the clipper to clip our primary surface
        push    OFFSET  lpClipper
        calldd  lpMainsurf,IDIRECTDRAWSURFACE_SETCLIPPER
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF

        call    MakeClientFit

    ; Create the backbuffer surface
        mov     ddsd.dwSize,SIZEOF ddsd
        mov     ddsd.dwFlags,(DDSD_CAPS or DDSD_HEIGHT or DDSD_WIDTH)
        m2m     ddsd.dwWidth,nXres
        m2m     ddsd.dwHeight,nYres
        mov     ddsd.ddsCaps.dwCaps,DDSCAPS_OFFSCREENPLAIN
        calldd  lpdd,IDIRECTDRAW_CREATESURFACE
        .IF eax!=DD_OK
            INVOKE  gfxShutdown
            return  FALSE
        .ENDIF
    .ENDIF
gfxInit         Endp

gfxShutdown proc    STDCALL

gfxShutdown Endp

End LibMain
