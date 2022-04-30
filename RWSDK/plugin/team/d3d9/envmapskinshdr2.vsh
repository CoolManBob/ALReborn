vs_2_0

#include "skindefs.h" 

dcl_position0 VSIN_REG_POS
dcl_blendweight0 VSIN_REG_WEIGHTS
dcl_blendindices0 VSIN_REG_INDICES
dcl_normal0 VSIN_REG_NORMAL
dcl_texcoord0 VSIN_REG_TEXCOORDS

;------------------------------------------------------------------------------
; Bone space transforms
;------------------------------------------------------------------------------

    mova    a0.xy, VSIN_REG_INDICES

    ; Bone 0 ------------------------------------------------------------------------------------
    mul     VSTMP_REG_SKIN_TRANS_0, VSIN_REG_WEIGHTS.x, c[a0.x + 0 + VSCONST_REG_MATRIX_OFFSET]
    mul     VSTMP_REG_SKIN_TRANS_1, VSIN_REG_WEIGHTS.x, c[a0.x + 1 + VSCONST_REG_MATRIX_OFFSET]
    mul     VSTMP_REG_SKIN_TRANS_2, VSIN_REG_WEIGHTS.x, c[a0.x + 2 + VSCONST_REG_MATRIX_OFFSET]

    ; Bone 1 ------------------------------------------------------------------------------------
    mad     VSTMP_REG_SKIN_TRANS_0, VSIN_REG_WEIGHTS.y, c[a0.y + 0 + VSCONST_REG_MATRIX_OFFSET], VSTMP_REG_SKIN_TRANS_0
    mad     VSTMP_REG_SKIN_TRANS_1, VSIN_REG_WEIGHTS.y, c[a0.y + 1 + VSCONST_REG_MATRIX_OFFSET], VSTMP_REG_SKIN_TRANS_1
    mad     VSTMP_REG_SKIN_TRANS_2, VSIN_REG_WEIGHTS.y, c[a0.y + 2 + VSCONST_REG_MATRIX_OFFSET], VSTMP_REG_SKIN_TRANS_2

    ; Skin position
    dp4     VSTMP_REG_POS_ACCUM.x, VSTMP_REG_SKIN_TRANS_0, VSIN_REG_POS
    dp4     VSTMP_REG_POS_ACCUM.y, VSTMP_REG_SKIN_TRANS_1, VSIN_REG_POS
    dp4     VSTMP_REG_POS_ACCUM.z, VSTMP_REG_SKIN_TRANS_2, VSIN_REG_POS

    ; Skin normal
    dp3     VSTMP_REG_NORMAL_ACCUM.x, VSTMP_REG_SKIN_TRANS_0, VSIN_REG_NORMAL
    dp3     VSTMP_REG_NORMAL_ACCUM.y, VSTMP_REG_SKIN_TRANS_1, VSIN_REG_NORMAL
    dp3     VSTMP_REG_NORMAL_ACCUM.z, VSTMP_REG_SKIN_TRANS_2, VSIN_REG_NORMAL

    ; Copy w - should be 1 in VSIN_REG_POS by default, but can't go through blending stuff.
    mov		VSTMP_REG_POS_ACCUM.w, VSIN_REG_POS.w

;------------------------------------------------------------------------------
; Combined camera & projection matrix
;------------------------------------------------------------------------------

    dp4		oPos.x, VSTMP_REG_POS_ACCUM, VSCONST_REG_TRANSFORM_X
    dp4		oPos.y, VSTMP_REG_POS_ACCUM, VSCONST_REG_TRANSFORM_Y
    dp4		oPos.z, VSTMP_REG_POS_ACCUM, VSCONST_REG_TRANSFORM_Z
    dp4     oPos.w, VSTMP_REG_POS_ACCUM, VSCONST_REG_TRANSFORM_W

;------------------------------------------------------------------------------
; Fogging - remove it if you don't use it ever to save a cycle
;------------------------------------------------------------------------------
; Can't read from the oPos register so I need to repeat the op for oPos.w
    dp4     oFog, VSTMP_REG_POS_ACCUM, VSCONST_REG_TRANSFORM_W

;------------------------------------------------------------------------------
; Single directional light + Ambient
;------------------------------------------------------------------------------

    ; Normalize
    dp3     VSTMP_REG_NORMAL_ACCUM.w, VSTMP_REG_NORMAL_ACCUM, VSTMP_REG_NORMAL_ACCUM
    rsq     VSTMP_REG_NORMAL_ACCUM.w, VSTMP_REG_NORMAL_ACCUM.w
    mul     VSTMP_REG_NORMAL_ACCUM.xyz, VSTMP_REG_NORMAL_ACCUM.xyz, VSTMP_REG_NORMAL_ACCUM.w

    ; DP normal & light dir clamp then scale by light color
    dp3     VSTMP_REG_NORMAL_ACCUM.w, VSTMP_REG_NORMAL_ACCUM, VSCONST_REG_LIGHT_DIR
    max     VSTMP_REG_NORMAL_ACCUM.w, VSCONST_REG_LIGHT_DIR.w, -VSTMP_REG_NORMAL_ACCUM.w

;------------------------------------------------------------------------------
;   Multiply diffuse light color by diffuse light coefficient & add ambient
;------------------------------------------------------------------------------

    mov     VSTMP_REG_COLOR_TMP, VSCONST_REG_AMBIENT

    mad     oD0.xyz, VSCONST_REG_LIGHT_COLOR.xyz, VSTMP_REG_NORMAL_ACCUM.w, VSTMP_REG_COLOR_TMP.xyz
    mov     oD0.w, VSCONST_REG_AMBIENT.w

;------------------------------------------------------------------------------
; Copy texture coordinates
;------------------------------------------------------------------------------

    mov     oT0, VSIN_REG_TEXCOORDS

;------------------------------------------------------------------------------
; Calculate envmap texture coordinates
;------------------------------------------------------------------------------

    mov		VSTMP_REG_NORMAL_ACCUM.w, VSIN_REG_POS.w
    dp4     oT1.x, VSTMP_REG_NORMAL_ACCUM, c[VSCONST_REG_ENV_OFFSET + 0]
    dp4     oT1.y, VSTMP_REG_NORMAL_ACCUM, c[VSCONST_REG_ENV_OFFSET + 1]