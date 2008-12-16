#define LOGICAL int
#define DMAX(A,B) (A)>(B)?(A):(B)

int
PHI (LOGICAL L, int X1, int X2)
{
    return L ? X1 : X2;
}

void
SIMD_LOAD_V4SF (float *VEC, float *BASE)
{
    VEC[0] = BASE[0];
    VEC[1] = BASE[1];
    VEC[2] = BASE[2];
    VEC[3] = BASE[3];
}

void
SIMD_LOAD_GENERIC_V4SF (float *VEC, float X0, float X1, float X2, float X3)
{

    VEC[0] = X0;
    VEC[1] = X1;
    VEC[2] = X2;
    VEC[3] = X3;
}

void
SIMD_LOAD_CONSTANT_V4SF (float *VEC, float X0, float X1, float X2, float X3)
{

    VEC[0] = X0;
    VEC[1] = X1;
    VEC[2] = X2;
    VEC[3] = X3;
}

void
SIMD_SAVE_V4SF (float *VEC, float *BASE)
{  BASE[0] = VEC[0];
    BASE[1] = VEC[1];
    BASE[2] = VEC[2];
    BASE[3] = VEC[3];
}

void
SIMD_SAVE_GENERIC_V4SF (float *VEC, float *X1, float *X2,
        float *X3, float *X4)
{

    *X1 = VEC[0];
    *X2 = VEC[1];
    *X3 = VEC[2];
    *X4 = VEC[3];
}

void
SIMD_CMPGTPS (LOGICAL * DEST, float *SRC1, float *SRC2)
{
    DEST[0] = SRC1[0] > SRC2[0];
    DEST[1] = SRC1[1] > SRC2[1];
    DEST[2] = SRC1[2] > SRC2[2];
    DEST[3] = SRC1[3] > SRC2[3];
}

void
SIMD_PHIPS (float *DEST, LOGICAL * COND, float *SRC1, float *SRC2)
{

    if (COND[0])
    {
        DEST[0] = SRC1[0];
    }
    else
    {
        DEST[0] = SRC2[0];
    }
    if (COND[1])
    {
        DEST[1] = SRC1[1];
    }
    else
    {
        DEST[1] = SRC2[1];
    }
    if (COND[2])
    {
        DEST[2] = SRC1[2];
    }
    else
    {
        DEST[2] = SRC2[2];
    }
    if (COND[3])
    {
        DEST[3] = SRC1[3];
    }
    else
    {
        DEST[3] = SRC2[3];
    }
}

void
SIMD_ADDPS (float *DEST, float *SRC1, float *SRC2)
{
    DEST[0] = SRC1[0] + SRC2[0];
    DEST[1] = SRC1[1] + SRC2[1];
    DEST[2] = SRC1[2] + SRC2[2];
    DEST[3] = SRC1[3] + SRC2[3];
}

void
SIMD_SUBPS (float *DEST, float *SRC1, float *SRC2)
{
    DEST[0] = SRC1[0] - SRC2[0];
    DEST[1] = SRC1[1] - SRC2[1];
    DEST[2] = SRC1[2] - SRC2[2];
    DEST[3] = SRC1[3] - SRC2[3];
}

void
SIMD_MULPS (float *DEST, float *SRC1, float *SRC2)
{
    DEST[0] = SRC1[0] * SRC2[0];
    DEST[1] = SRC1[1] * SRC2[1];
    DEST[2] = SRC1[2] * SRC2[2];
    DEST[3] = SRC1[3] * SRC2[3];
}

void
SIMD_DIVPS (float *DEST, float *SRC1, float *SRC2)
{
    DEST[0] = SRC1[0] / SRC2[0];
    DEST[1] = SRC1[1] / SRC2[1];
    DEST[2] = SRC1[2] / SRC2[2];
    DEST[3] = SRC1[3] / SRC2[3];
}

void
SIMD_MAXPS (float *DEST, float *SRC1, float *SRC2)
{
    DEST[0] = DMAX (SRC1[0], SRC2[0]);
    DEST[1] = DMAX (SRC1[1], SRC2[1]);
    DEST[2] = DMAX (SRC1[2], SRC2[2]);
    DEST[3] = DMAX (SRC1[3], SRC2[3]);
}

void
SIMD_LOAD_CONSTANT_V2SI (int *VEC, int HIGH, int LOW)
{

    VEC[0] = LOW;
    VEC[1] = HIGH;
}

void
SIMD_LOAD_V2SI (int* VEC, int* BASE)
{  
    VEC[0] = BASE[0];
    VEC[1] = BASE[1];
}

void
SIMD_LOAD_GENERIC_V2SI (int *VEC, int X1, int X2)
{

    VEC[0] = X1;
    VEC[1] = X2;
}

void
SIMD_SAVE_V2SI (int *VEC, int *BASE)
{  BASE[0] = VEC[0];
    BASE[1] = VEC[1];
}

void
SIMD_SAVE_GENERIC_V2SI (int *VEC, int *X1, int *X2)
{

    *X1 = VEC[0];
    *X2 = VEC[1];
}

void
SIMD_LOAD_V4SI (int *VEC, int *BASE)
{  VEC[0] = BASE[0];
    VEC[1] = BASE[1];
    VEC[2] = BASE[2];
    VEC[3] = BASE[3];
}

void
SIMD_LOAD_GENERIC_V4SI (int *VEC, int X1, int X2, int X3, int X4)
{

    VEC[0] = X1;
    VEC[1] = X2;
    VEC[2] = X3;
    VEC[3] = X4;
}

void
SIMD_SAVE_V4SI (int *VEC, int *BASE)
{  BASE[0] = VEC[0];
    BASE[1] = VEC[1];
    BASE[2] = VEC[2];
    BASE[3] = VEC[3];
}

void
SIMD_SAVE_GENERIC_V4SI (int *VEC, int *X1, int *X2, int *X3, int *X4)
{

    *X1 = VEC[0];
    *X2 = VEC[1];
    *X3 = VEC[2];
    *X4 = VEC[3];
}

void
SIMD_ADDD (int *DEST, int *SRC1, int *SRC2)
{
    DEST[0] = SRC1[0] + SRC2[0];
    DEST[1] = SRC1[1] + SRC2[1];
}

void
SIMD_SUBD (int *DEST, int *SRC1, int *SRC2)
{
    DEST[0] = SRC1[0] - SRC2[0];
    DEST[1] = SRC1[1] - SRC2[1];
}

void
SIMD_MULD (int *DEST, int *SRC1, int *SRC2)
{
    DEST[0] = SRC1[0] * SRC2[0];
    DEST[1] = SRC1[1] * SRC2[1];
}

void
SIMD_LOAD_CONSTANT_V4HI (short *VEC, int HIGH, int LOW)
{
    VEC[0] = (short) LOW;
    VEC[1] = (short) (LOW >> 2);
    VEC[2] = (short) HIGH;
    VEC[3] = (short) (HIGH >> 2);
}

void
SIMD_LOAD_V4HI (short *VEC, short *BASE)
{  VEC[0] = BASE[0];
    VEC[1] = BASE[1];
    VEC[2] = BASE[2];
    VEC[3] = BASE[3];
}

void
SIMD_LOAD_V4QI_TO_V4HI (short *VEC, char *BASE)
{  VEC[0] = BASE[0];
    VEC[1] = BASE[1];
    VEC[2] = BASE[2];
    VEC[3] = BASE[3];
}

void
SIMD_LOAD_GENERIC_V4HI (short *VEC, short X1, short X2, short X3, short X4)
{

    VEC[0] = X1;
    VEC[1] = X2;
    VEC[2] = X3;
    VEC[3] = X4;
}

void
SIMD_SAVE_V4HI (short *VEC, short *BASE)
{  BASE[0] = VEC[0];
    BASE[1] = VEC[1];
    BASE[2] = VEC[2];
    BASE[3] = VEC[3];
}

void
SIMD_SAVE_GENERIC_V4HI (short *VEC, short *X1,
        short *X2, short *X3, short *X4)
{

    *X1 = VEC[0];
    *X2 = VEC[1];
    *X3 = VEC[2];
    *X4 = VEC[3];
}

void
SIMD_CMPGTW (LOGICAL * DEST, short *SRC1, short *SRC2)
{
    DEST[0] = SRC1[0] > SRC2[0];
    DEST[1] = SRC1[1] > SRC2[1];
    DEST[2] = SRC1[2] > SRC2[2];
    DEST[3] = SRC1[3] > SRC2[3];
}

void
SIMD_PHIW (short *DEST, LOGICAL * COND, short *SRC1, short *SRC2)
{

    if (COND[0])
    {
        DEST[0] = SRC1[0];
    }
    else
    {
        DEST[0] = SRC2[0];
    }
    if (COND[1])
    {
        DEST[1] = SRC1[1];
    }
    else
    {
        DEST[1] = SRC2[1];
    }
    if (COND[2])
    {
        DEST[2] = SRC1[2];
    }
    else
    {
        DEST[2] = SRC2[2];
    }
    if (COND[3])
    {
        DEST[3] = SRC1[3];
    }
    else
    {
        DEST[3] = SRC2[3];
    }
}

void
SIMD_ADDW (short *DEST, short *SRC1, short *SRC2)
{
    DEST[0] = SRC1[0] + SRC2[0];
    DEST[1] = SRC1[1] + SRC2[1];
    DEST[2] = SRC1[2] + SRC2[2];
    DEST[3] = SRC1[3] + SRC2[3];
}

void
SIMD_SUBW (short *DEST, short *SRC1, short *SRC2)
{
    DEST[0] = SRC1[0] - SRC2[0];
    DEST[1] = SRC1[1] - SRC2[1];
    DEST[2] = SRC1[2] - SRC2[2];
    DEST[3] = SRC1[3] - SRC2[3];
}

void
SIMD_MULW (short *DEST, short *SRC1, short *SRC2)
{
    DEST[0] = SRC1[0] * SRC2[0];
    DEST[1] = SRC1[1] * SRC2[1];
    DEST[2] = SRC1[2] * SRC2[2];
    DEST[3] = SRC1[3] * SRC2[3];
}

void
SIMD_LOAD_CONSTANT_V8QI (char *VEC, int HIGH, int LOW)
{
    VEC[0] = (char) LOW;
    VEC[1] = (char) (LOW >> 1);
    VEC[2] = (char) (LOW >> 2);
    VEC[3] = (char) (LOW >> 3);
    VEC[4] = (char) HIGH;
    VEC[5] = (char) (HIGH >> 1);
    VEC[6] = (char) (HIGH >> 2);
    VEC[7] = (char) (HIGH >> 3);
}

void
SIMD_LOAD_V8QI (char *VEC, char *BASE)
{  VEC[0] = BASE[0];
    VEC[1] = BASE[1];
    VEC[2] = BASE[2];
    VEC[3] = BASE[3];
    VEC[4] = BASE[4];
    VEC[5] = BASE[5];
    VEC[6] = BASE[6];
    VEC[7] = BASE[7];
}

void
SIMD_LOAD_GENERIC_V8QI (char *VEC, char X1,
        char X2, char X3, char X4, char X5, char X6,
        char X7, char X8)
{
    VEC[0] = X1;
    VEC[1] = X2;
    VEC[2] = X3;
    VEC[3] = X4;
    VEC[4] = X5;
    VEC[5] = X6;
    VEC[6] = X7;
    VEC[7] = X8;
}

void
SIMD_SAVE_V8QI (char *VEC, char *BASE)
{  BASE[0] = VEC[0];
    BASE[1] = VEC[1];
    BASE[2] = VEC[2];
    BASE[3] = VEC[3];
    BASE[4] = VEC[4];
    BASE[5] = VEC[5];
    BASE[6] = VEC[6];
    BASE[7] = VEC[7];
}

void
SIMD_SAVE_GENERIC_V8QI (char *VEC, char *X0,
        char *X1, char *X2, char *X3, char *X4, char *X5,
        char *X6, char *X7)
{

    *X0 = VEC[0];
    *X1 = VEC[1];
    *X2 = VEC[2];
    *X3 = VEC[3];
    *X4 = VEC[4];
    *X5 = VEC[5];
    *X6 = VEC[6];
    *X7 = VEC[7];
}

void
SIMD_ADDB (char *DEST, char *SRC1, char *SRC2)
{
    DEST[0] = SRC1[0] + SRC2[0];
    DEST[1] = SRC1[1] + SRC2[1];
    DEST[2] = SRC1[2] + SRC2[2];
    DEST[3] = SRC1[3] + SRC2[3];
    DEST[4] = SRC1[4] + SRC2[4];
    DEST[5] = SRC1[5] + SRC2[5];
    DEST[6] = SRC1[6] + SRC2[6];
    DEST[7] = SRC1[7] + SRC2[7];
}

void
SIMD_SUBB (char *DEST, char *SRC1, char *SRC2)
{
    DEST[0] = SRC1[0] - SRC2[0];
    DEST[1] = SRC1[1] - SRC2[1];
    DEST[2] = SRC1[2] - SRC2[2];
    DEST[3] = SRC1[3] - SRC2[3];
    DEST[4] = SRC1[4] - SRC2[4];
    DEST[5] = SRC1[5] - SRC2[5];
    DEST[6] = SRC1[6] - SRC2[6];
    DEST[7] = SRC1[7] - SRC2[7];
}

void
SIMD_MULB (char *DEST, char *SRC1, char *SRC2)
{

    DEST[0] = SRC1[0] * SRC2[0];
    DEST[1] = SRC1[1] * SRC2[1];
    DEST[2] = SRC1[2] * SRC2[2];
    DEST[3] = SRC1[3] * SRC2[3];
    DEST[4] = SRC1[4] * SRC2[4];
    DEST[5] = SRC1[5] * SRC2[5];
    DEST[6] = SRC1[6] * SRC2[6];
    DEST[7] = SRC1[7] * SRC2[7];
}

void
SIMD_MOVPS (float *DEST, float *SRC)
{
    DEST[0] = SRC[0];
    DEST[1] = SRC[1];
}

void
SIMD_MOVD (int *DEST, int *SRC)
{
    DEST[0] = SRC[0];
    DEST[1] = SRC[1];
}

void
SIMD_MOVW (short *DEST, short *SRC)
{
    DEST[0] = SRC[0];
    DEST[1] = SRC[1];
    DEST[2] = SRC[2];
    DEST[3] = SRC[3];
}

void
SIMD_MOVB (char *DEST, char *SRC)
{

    DEST[0] = SRC[0];
    DEST[1] = SRC[1];
    DEST[2] = SRC[2];
    DEST[3] = SRC[3];
    DEST[4] = SRC[4];
    DEST[5] = SRC[5];
    DEST[6] = SRC[6];
    DEST[7] = SRC[7];
}

void
SIMD_OPPPS (float *DEST, float *SRC)
{
    DEST[0] = -SRC[0];
    DEST[1] = -SRC[1];
}

void
SIMD_OPPD (int *DEST, int *SRC)
{
    DEST[0] = -SRC[0];
    DEST[1] = -SRC[1];
}

void
SIMD_OPPW (short *DEST, short *SRC)
{
    DEST[0] = -SRC[0];
    DEST[1] = -SRC[1];
    DEST[2] = -SRC[2];
    DEST[3] = -SRC[3];
}

void
SIMD_OPPB (char *DEST, char *SRC)
{
    DEST[0] = -SRC[0];
    DEST[1] = -SRC[1];
    DEST[2] = -SRC[2];
    DEST[3] = -SRC[3];
    DEST[4] = -SRC[4];
    DEST[5] = -SRC[5];
    DEST[6] = -SRC[6];
    DEST[7] = -SRC[7];
}

#undef LOGICAL
#undef DMAX
