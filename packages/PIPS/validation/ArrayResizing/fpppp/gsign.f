      FUNCTION GSIGN (ARG1,ARG2)
C
C     TRANSFER OF SIGN ... RESULT IS DABS(ARG1) * SIGN OF ARG2
C
      REAL*8 GSIGN,DSIGN,ARG1,ARG2
C
      GSIGN = DSIGN (ARG1,ARG2)
      RETURN
      END
