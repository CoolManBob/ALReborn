#ifndef BLUR_VSHADER_DEFS_H
#define BLUR_VSHADER_DEFS_H

/* For use in the vertex shader descriptor */
#define VSD_REG_POS                     0
#define VSD_REG_TEXCOORDS               1

/* Input register - For use in the vertex shader code */
#define VSIN_REG_POS                    v0
#define VSIN_REG_TEXCOORDS              v1

/* Vertex shader defines */
#define VSCONST_REG_BASE                0

#define VSCONST_REG_TRANSFORM_OFFSET    VSCONST_REG_BASE
#define VSCONST_REG_TRANSFORM_SIZE      4

#define VSCONST_REG_T0_OFFSET           VSCONST_REG_TRANSFORM_OFFSET + VSCONST_REG_TRANSFORM_SIZE
#define VSCONST_REG_T0_SIZE             1
#define VSCONST_REG_T1_OFFSET           VSCONST_REG_T0_OFFSET + VSCONST_REG_T0_SIZE
#define VSCONST_REG_T1_SIZE             1
#define VSCONST_REG_T2_OFFSET           VSCONST_REG_T1_OFFSET + VSCONST_REG_T1_SIZE
#define VSCONST_REG_T2_SIZE             1
#define VSCONST_REG_T3_OFFSET           VSCONST_REG_T2_OFFSET + VSCONST_REG_T2_SIZE
#define VSCONST_REG_T3_SIZE             1

/* Constant register - For use in the vertex shader code */
#define VSCONST_REG_TRANSFORM_X         c[VSCONST_REG_TRANSFORM_OFFSET + 0]
#define VSCONST_REG_TRANSFORM_Y         c[VSCONST_REG_TRANSFORM_OFFSET + 1]
#define VSCONST_REG_TRANSFORM_Z         c[VSCONST_REG_TRANSFORM_OFFSET + 2]
#define VSCONST_REG_TRANSFORM_W         c[VSCONST_REG_TRANSFORM_OFFSET + 3]

#define VSCONST_REG_T0                  c[VSCONST_REG_T0_OFFSET]
#define VSCONST_REG_T1                  c[VSCONST_REG_T1_OFFSET]
#define VSCONST_REG_T2                  c[VSCONST_REG_T2_OFFSET]
#define VSCONST_REG_T3                  c[VSCONST_REG_T3_OFFSET]

#endif
