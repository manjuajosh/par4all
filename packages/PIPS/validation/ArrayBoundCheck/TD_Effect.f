* Example with write effect on variable 
* of the bound of array's declaration

      SUBROUTINE EFFECT(A, N)
      
      REAL A(N)
        
      N = 2*N
      DO I = 1, N
         A(I) = 0.
      ENDDO
        
      END
