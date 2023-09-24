c-----------------------------------------------------------------------
      subroutine nek_ascent_setup()
#ifdef ASCENT
      call ascent_setup()
#endif
      end
c-----------------------------------------------------------------------
      subroutine nek_ascent_update()
#ifdef ASCENT
      call ascent_update()
#endif
      end
c-----------------------------------------------------------------------
      subroutine nek_ascent_finalize()
#ifdef ASCENT
      call ascent_finalize()
#endif
      end
c-----------------------------------------------------------------------