c-----------------------------------------------------------------------
      subroutine nek_ascent_setup()
#ifdef ASCENT
      include 'SIZE'
      include 'TOTAL'
      common /nekmpi/ mid,mp,nekcomm,nekgroup,nekreal

      call ascent_setup(nekcomm, CASENAME//char(0), nelt,
     &       lx1, ly1, lz1, xm1, ym1, zm1,
     &       lx2, ly2, lz2, xm2, ym2, zm2,
     &       vx, vy, vz, pr, t, jacm1, ldimt)
#endif
      end
c-----------------------------------------------------------------------
      subroutine nek_ascent_update()
#ifdef ASCENT
      include 'SIZE'
      include 'TOTAL'

      call ascent_update(istep, time)
#endif
      end
c-----------------------------------------------------------------------
      subroutine nek_ascent_finalize()
#ifdef ASCENT
      call ascent_finalize()
#endif
      end
c-----------------------------------------------------------------------