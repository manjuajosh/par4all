      PRINT *, LORDRE(0)
      END
      FUNCTION  LORDRE( L1 )
      INTEGER L1, LORDRE, I
      LORDRE=0
      DO 10 I=1,L
         IF (L1.EQ.I) GOTO 15
   10 CONTINUE
      STOP 100
   15 LORDRE=I
      RETURN
      END
