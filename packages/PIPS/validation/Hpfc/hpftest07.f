      program hpftest07
      integer g(10)
      integer j
CHPF$ TEMPLATE t(10)
CHPF$ ALIGN g(I) with t(I)
CHPF$ PROCESSORS p(2)
CHPF$ DISTRIBUTE t(block) ONTO p
      j=2
      g(1)=1
CHPF$ INDEPENDENT(I)
      do i=j,10
         g(g(i-1))=i
      enddo
      if (g(g(g(1)+1)-1).EQ.g(1)) g(g(1))=1000
      if (j.EQ.1) j=1000
      end
