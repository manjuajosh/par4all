      real function sum(n, a)
      real s, a(100)
      s  = 0.
      do i = 0, n
         s = s + 2. * a(i)
      enddo
      sum = s
      end
