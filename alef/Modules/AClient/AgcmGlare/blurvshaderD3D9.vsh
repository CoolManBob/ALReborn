vs_1_1

#include "blurvshaderdefs.h"

dcl_position0 VSIN_REG_POS
dcl_texcoord0 VSIN_REG_TEXCOORDS

;------------------------------------------------------------------------------
; Transform the position by the combined world, camera & projection matrix 
;------------------------------------------------------------------------------

dp4 oPos.x, VSIN_REG_POS, VSCONST_REG_TRANSFORM_X
dp4 oPos.y, VSIN_REG_POS, VSCONST_REG_TRANSFORM_Y
dp4 oPos.z, VSIN_REG_POS, VSCONST_REG_TRANSFORM_Z
dp4 oPos.w, VSIN_REG_POS, VSCONST_REG_TRANSFORM_W

;------------------------------------------------------------------------------
; Offset the texture coordinates & copy them to the 4 texture stages 
;------------------------------------------------------------------------------

; mov oT0, VSIN_REG_TEXCOORDS

add oT0, VSIN_REG_TEXCOORDS, VSCONST_REG_T0
add oT1, VSIN_REG_TEXCOORDS, VSCONST_REG_T1
add oT2, VSIN_REG_TEXCOORDS, VSCONST_REG_T2
add oT3, VSIN_REG_TEXCOORDS, VSCONST_REG_T3
