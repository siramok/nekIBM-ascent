c-----------------------------------------------------------------------
      subroutine lpm_user_marker_distribution
      include 'SIZE'
      include 'TOTAL'
      include 'LPM'

      return
      end   
c-----------------------------------------------------------------------
      subroutine lpm_usr_f
      include 'SIZE'
      include 'TOTAL'
      include 'LPM'
      parameter(rgrav = 9.8) ! gravitational acceleration

      ! uncoupled gravity in -y direction and buoyancy 
      lpmforce(1) = 0.0  
      lpmforce(2) = 0.0 !-rgrav*lpmvol_p*(lpmdens_p-lpmdens_f)
      lpmforce(3) = 0.0
     
      ! coupled user forces
      lpmforcec(1) =  0.   ! - (lpmv_p(1) - lpmv_f(1)) / dt 
      lpmforcec(2) =  0.   ! - (lpmv_p(2) - lpmv_f(2)) / dt 
      lpmforcec(3) =  0.   ! - (lpmv_p(3) - lpmv_f(3)) / dt 

c      write(6,*), lpmv_p(2), lpmv_f(2), dt
c      write(6,*) "coupled forces",lpmforcec(1),lpmforcec(2),lpmforcec(3)

c     if (lpmx_p(2)+lpmdiam_p/2.0 .gt. 0.03) lpmx_p(2) = -1

      return
      end

c-----------------------------------------------------------------------
      subroutine uservp (ix,iy,iz,eg)
      include 'SIZE'
      include 'TOTAL'   ! this is not
      include 'NEKUSE'
      integer e,eg

      e = gllel(eg)

      udiff=0.0
      utrans=0.

      return
      end

c-----------------------------------------------------------------------
      subroutine userf  (ix,iy,iz,eg)
      include 'SIZE'
      include 'TOTAL'
      include 'NEKUSE'
      integer e,eg

      ffx = 0.
      ffy = 0.
      ffz = 0.

      return
      end
c-----------------------------------------------------------------------
      subroutine userq  (ix,iy,iz,eg)
      include 'SIZE'
      include 'TOTAL'
      include 'NEKUSE'
      integer e,eg

      qvol   = 0.0

      return
      end
c-----------------------------------------------------------------------
      subroutine userchk
      include 'SIZE'
      include 'TOTAL'
      include 'LPM'
      integer  e,f

      ifxyo=.true.
      if (istep.gt.1) ifxyo=.false.

      ifIBM = 1
      ibm_debug     = 1
      ibm_debug_queen  = 0
      ibm_debug_worker = 0
      ibm_debug_bin    = 0
      ibm_debug_col    = 0
      ioutput_debug    = 0
      ioutput_queen    = 1
      ibm_rotation     = 0

      num_of_IBMpart       = 1  ! particle number
      IBM_Particle_shape   = 2  ! Particle type 1 - 2D circular disk, 2 3D sphere;
      IBM_marker_shape     = 2  ! marker volume 1 - 2D circular disk, 2 - sphere, 0 traditional particle
      ipart_stationary     = 1  ! stationary particles for debug only
      ipart_moving     	   = 0  ! stationary particles for debug only

      do nn = 1, num_of_IBMpart
         n_dh(nn) = 20    
         ibm_center(nn,1) = 0.0
         ibm_center(nn,2) = 0.0
         ibm_center(nn,3) = 0.0
         ibm_diam(nn) = 0.01
      enddo

      if (mod(istep,iostep).eq.0) then
            call  divergence_vel
      endif

      interm_vel = 1
      !     =  0 use previous step veloicity;
      !     = 1 explicit;
      !     = 2 implicit
      if(interm_vel.ge.1) call ibm_interm_vel


      return
      end
c-----------------------------------------------------------------------
      subroutine userbc (ix,iy,iz,iside,eg)
      include 'SIZE'
      include 'TSTEP'
      include 'NEKUSE'
      include 'INPUT'
      include 'GEOM' 

      common /bed_measures/ rpavg,ruy

      ruy = 1

      ux = 0.0
      uy = ruy
      uz = 0.0

      return
      end
c-----------------------------------------------------------------------

      subroutine useric (ix,iy,iz,eg)
      include 'SIZE'
      include 'TOTAL'
      include 'NEKUSE'
      integer e,eg, eqnum

      common /bed_measures/ rpavg,ruy


      ux = 0.0
      uy = ruy
      uz = 0.0

      return
      end
c-----------------------------------------------------------------------
      subroutine usrdat
      include 'SIZE'
      include 'TOTAL'

      return
      end
c-----------------------------------------------------------------------
      subroutine usrdat2
      include 'SIZE'
      include 'TOTAL'

      return
      end
!-----------------------------------------------------------------------
      subroutine usrdat3
      return
      end
c-----------------------------------------------------------------------

c----------------------------------------------------------------------------
      subroutine ibm_interm_vel !(stage)
c-----------------------------------------------------------------------------
c     computer intermediate velocity for force calculation
c     
      include 'SIZE'
      include 'TOTAL'
      include 'LPM'

      if(nid.eq.0)write(*,*)"Start Interm Vel Solver"

      ibm_force_gate = 1        ! default 

      if (interm_vel .eq. 1) then
         call compute_interm_vel_explicit
      else if (interm_vel .eq. 2) then 
         call compute_interm_vel_implicit
      else
         call copy(vx_tilde, vx_e, ntot1)
         call copy(vy_tilde, vy_e, ntot1)
         call copy(vz_tilde, vz_e, ntot1)
      endif

      return
      end subroutine ibm_interm_vel
c-----------------------------------------------------------------------


c-----------------------------------------------------------------------
      subroutine conv_interm
c-----------------------------------------------------------------------
c     convective term for intermediate velocity solver
c 	  (u Grad) u
c 	  input: u_x, u_y, u_z
c	  output: conv_x, conv_y, conv_z
	  
	   include 'SIZE'
	   include 'TOTAL'
	   include 'LPM'

	   real u_xx (lx1,ly1,lz1,lelt)
	   real u_xy (lx1,ly1,lz1,lelt)
	   real u_xz (lx1,ly1,lz1,lelt)

	   real u_yx (lx1,ly1,lz1,lelt)
	   real u_yy (lx1,ly1,lz1,lelt)
	   real u_yz (lx1,ly1,lz1,lelt)

	   real u_zx (lx1,ly1,lz1,lelt)
	   real u_zy (lx1,ly1,lz1,lelt)
	   real u_zz (lx1,ly1,lz1,lelt)
		  
	   ntot1 = lx1*ly1*lz1*lelt

           call rzero(u_xx, ntot1)
           call rzero(u_xy, ntot1)
           call rzero(u_xz, ntot1)
           call rzero(u_yx, ntot1)
           call rzero(u_yy, ntot1)
           call rzero(u_yz, ntot1)
           call rzero(u_zx, ntot1)
           call rzero(u_zy, ntot1)
           call rzero(u_zz, ntot1)

           call rzero(conv_x, ntot1)
           call rzero(conv_y, ntot1)
           call rzero(conv_z, ntot1)
                      
c	compute gradient
	   call gradm1(u_xx,u_xy,u_xz,vx)
	   call gradm1(u_yx,u_yy,u_yz,vy)
	   call gradm1(u_zx,u_zy,u_zz,vz)
		  
c	compute convective item
c      conv_x = vx*u_xx + vy*u_xy + vz*u_xz
c      conv_y = vx*u_yx + vy*u_yy + vz*u_yz
c      conv_z = vx*u_zx + vy*u_zy + vz*u_zz

	   call col2(u_xx, vx, ntot1)
	   call col2(u_xy, vy, ntot1)
	   if(if3d) call col2(u_xz, vz, ntot1)
	   call add4(conv_x, u_xx, u_xy, u_xz, ntot1)

	   call col2(u_yx, vx, ntot1)
	   call col2(u_yy, vy, ntot1)
	   if(if3d)  call col2(u_yz, vz, ntot1)
	   call add4(conv_y, u_yx, u_yy, u_yz, ntot1)

           if(if3d) then
              call col2(u_zx, vx, ntot1)
              call col2(u_zy, vy, ntot1)
              call col2(u_zz, vz, ntot1)
              call add4(conv_z, u_zx, u_zy, u_zz, ntot1)
           endif
           
	return
	end subroutine conv_interm
      
c---------------------------------------------------------------------	
       subroutine gradpress_interm
c---------------------------------------------------------------------	
c     compute the pressure gradient and velocity gradient
c	  Grad(p) -> vector
c     Input: pr
c     Output: dpdx_interm, dpdy_interm, dpdz_interm

         include 'SIZE'
         include 'TOTAL'
         include 'LPM'

         real pm1(lx1,ly1,lz1,lelt)
     $        ,px(lx1,ly1,lz1,lelt) !
     $        ,py(lx1,ly1,lz1,lelt) !

         if (lx2.ne.lx1) then	! use something different for mesh 2
            call mappr (pm1,pr,px,py) ! interpolate Pr --> P(m1)
            call gradm1(dpdx_interm,dpdy_interm,dpdz_interm,pm1)
         else
            call gradm1(dpdx_interm,dpdy_interm,dpdz_interm,pr)
         endif
         
       return 
       end subroutine gradpress_interm

c---------------------------------------------------------------------	
      subroutine laplacian_interm
c---------------------------------------------------------------------
c     compute laplacian term intermediate velocity
c      nabla(U) = visco
c       Output: laplacianV1, laplacianV2, laplacianV3

        include 'SIZE'
	include 'TOTAL'
	include 'LPM'
				
	real dV1dx(lx1,ly1,lz1,lelt)
     &	    ,dV1dy(lx1,ly1,lz1,lelt) 
     &      ,dV1dz(lx1,ly1,lz1,lelt)
     &	    ,dV2dx(lx1,ly1,lz1,lelt)
     &      ,dV2dy(lx1,ly1,lz1,lelt)
     &      ,dV2dz(lx1,ly1,lz1,lelt)
     &      ,dV3dx(lx1,ly1,lz1,lelt)
     &      ,dV3dy(lx1,ly1,lz1,lelt)
     &      ,dV3dz(lx1,ly1,lz1,lelt)	 
     &	    ,work1(lx1,ly1,lz1,lelt)
     &	    ,work2(lx1,ly1,lz1,lelt)


        ntot1 = lx1*ly1*lz1*lelt
           
	visco = param(2)/param(1)
	! Reynolds = 1.0 / visco

        call rzero(laplacianV1,ntot1)
        call rzero(laplacianV2,ntot1)
        call rzero(laplacianV3,ntot1)

        
        call gradm1(dvxdx,dvxdy,dvxdz,vx) ! compute grad of dudx, dudy, dudz 
        call gradm1(dvydx,dvydy,dvydz,vy) ! compute grad of dvdx, dvdy, dvdz 
        call gradm1(dvzdx,dvzdy,dvzdz,vz) ! compute grad of dwdx, dwdy, dwdz  
	
!       gradVxx = uij(1,1,1)
!       gradVyy = uij(1,2,2)
!       gradVzz = uij(1,3,3)
		
	call gradm1(dV1dx,  work1, work2, dvxdx) ! dvxdx
	call gradm1(work1,  dV1dy, work2, dvxdy) ! dvxdy
	call gradm1(work1,  work2, dV1dz, dvxdz) ! dvxdz

	call add4  (laplacianV1, dV1dx, dV1dy, dV1dz, ntot1)
	call cmult (laplacianV1, visco, ntot1)

		
	call gradm1(dV2dx,  work1, work2, dvydx) ! 
	call gradm1(work1,  dV2dy, work2, dvydy) ! 
	call gradm1(work1,  work2, dV2dz, dvydz) ! 

	call add4  (laplacianV2, dV2dx, dV2dy, dV2dz, ntot1)
	call cmult (laplacianV2, visco, ntot1)
		
	call gradm1(dV3dx,  work1, work2, dvzdx) ! 
	call gradm1(work1,  dV3dy, work2, dvzdy) ! 
	call gradm1(work1,  work2, dV3dz, dvzdz) ! 

	call add4  (laplacianV3, dV3dx, dV3dy, dV3dz, ntot1)
	call cmult (laplacianV3, visco, ntot1)

      return 
      end subroutine laplacian_interm

      
c-------------------------------------------------------------------
      subroutine explict_time_iteration
c-------------------------------------------------------------------	   

        include 'SIZE'
	include 'TOTAL'
	include 'LPM'
      
	real dvx_interm(lx1,ly1,lz1,lelt)
     &     , dvy_interm(lx1,ly1,lz1,lelt)
     &     , dvz_interm(lx1,ly1,lz1,lelt)

        ntot1 = lx1*ly1*lz1*lelt

        visco = param(2)/param(1)

        !reynods = 1./param(2)	! kinematic viscosity

        ! x-dir
	call chsign(conv_x, ntot1) !
	call chsign(dpdx_interm, ntot1) !
	call cmult(dpdx_interm, visco, ntot1) !
	call add4(dvx_interm, conv_x, dpdx_interm, laplacianV1, ntot1) !

	call cmult(dvx_interm, dt, ntot1) !
	call add3(vx_tilde, vx, dvx_interm, ntot1)

        ! y-dir
	call chsign(conv_y, ntot1) !
	call chsign(dpdy_interm, ntot1) !
	call cmult(dpdy_interm, visco, ntot1) !
	call add4(dvy_interm, conv_y, dpdy_interm, laplacianV2, ntot1) !

	call cmult(dvy_interm, dt, ntot1) !
	call add3(vy_tilde, vy, dvy_interm, ntot1)

	if (if3d) then
! reynods = 1./param(2) ! kinematic viscosity
	   call chsign(conv_z, ntot1) !
	   call chsign(dpdz_interm, ntot1) !
	   call cmult(dpdz_interm, visco, ntot1) !
	   call add4(dvz_interm, conv_z, dpdz_interm, laplacianV3, ntot1) !
	   
	   call cmult(dvz_interm, dt, ntot1) !
	   call add3(vz_tilde, vz, dvz_interm, ntot1)
   
	endif

        return
        end subroutine explict_time_iteration 
c----------------------------------------------------------------------------------------
      

c-------------------------------------------------------------------
      subroutine compute_interm_vel_explicit
c-------------------------------------------------------------------	   
c       Input 
c       : laplacianV1, laplacianV2, laplacianV3
c       : ,dpdx_interm,dpdy_interm,dpdz_interm
c       : ,conv_x, conv_y, conv_z
c       vx_tilde = (- conv_x - dpdx_interm + laplacianV1) * dt + vx

        include 'SIZE'
	include 'TOTAL'
	include 'LPM'
        
        if(nid.eq.0)
     $   write(*,*) "--- Use explicit intermediate velocity solver ---"

c         write(*,*) "Step 0" 
	call gradpress_interm
c         write(*,*) "Step 1  pressure gradient "
	call conv_interm
c         write(*,*) "Step 2 convective "
	call laplacian_interm
c         write(*,*) "Step 3 laplacian"
        call explict_time_iteration
c         write(*,*) "Step 4 time iteration"

	return 
	end subroutine compute_interm_vel_explicit
c----------------------------------------------------------------------------------------

           

c-----------------------------------------------------------------------------      
c-----------------------------------------------------------------------------
      subroutine compute_interm_vel_implicit
c-----------------------------------------------------------------------------
c-----------------------------------------------------------------------------
      
      include 'SIZE'
      include 'TOTAL'
      include 'LPM'

      COMMON /SCRNS/  RESV1 (LX1,LY1,LZ1,LELV)
     $ ,              RESV2 (LX1,LY1,LZ1,LELV)
     $ ,              RESV3 (LX1,LY1,LZ1,LELV)
     $ ,              DV1   (LX1,LY1,LZ1,LELV)
     $ ,              DV2   (LX1,LY1,LZ1,LELV)
     $ ,              DV3   (LX1,LY1,LZ1,LELV)
      COMMON /SCRVH/  H1    (LX1,LY1,LZ1,LELV)
     $ ,              H2    (LX1,LY1,LZ1,LELV)
      
      ntot1 = lx1*ly1*lz1*nelt

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      write(*,*) "Use implicit intermediate velocity solver!"

!     Save Vx Vy Vz to Vx_tmp, dimension ntot1 = lx1*ly1*lz1*nelt 
      call opcopy(Vx_tmp, Vy_tmp, Vz_tmp, Vx, Vy, Vz) ! save previous step

      ! Save Vxlag, note dimension: vxlag (lx1,ly1,lz1,nelt,2) 
      DO  ILAG=1,2,-1
       CALL COPY (VXLAG_tmp (1,1,1,1,ILAG),VXLAG (1,1,1,1,ILAG),NTOT1)
       CALL COPY (VYLAG_tmp (1,1,1,1,ILAG),VYLAG (1,1,1,1,ILAG),NTOT1)
        IF (ldim.EQ.3)
     $ CALL COPY (VZLAG_tmp (1,1,1,1,ILAG),VZLAG (1,1,1,1,ILAG),NTOT1)
      ENDDO

      ! save pr prlag, dimension: p(lx2,ly2,lz2,1)
      ntot2 = lx2*ly2*lz2*nelv
      do i=1,ntot2
         pr_tmp     (i,1,1,1)            = pr     (i,1,1,1)
         prlag_tmp  (i,1,1,1,1)          = prlag  (i,1,1,1,1)
      enddo

      do i =1,ntot1
         abx2_tmp (i,1,1,1) = abx2(i,1,1,1)
         aby2_tmp (i,1,1,1) = aby2(i,1,1,1)
         abz2_tmp (i,1,1,1) = abz2(i,1,1,1)
         abx1_tmp (i,1,1,1) = abx1(i,1,1,1)
         aby1_tmp (i,1,1,1) = aby1(i,1,1,1)
         abz1_tmp (i,1,1,1) = abz1(i,1,1,1)
      enddo

      do i = 1,ntot1*(lorder-1)
         bm1lag_tmp(i,1,1,1,1) = bm1lag(i,1,1,1,1)       
      enddo

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
      ibm_force_gate = 1
      ! note
      ! ibm_force_gate = 0 for regular flow solver
      !                = 1, for intermediate velocity  

c     method 1
      call fluid(1)
      call fluid(2)

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ! restore Vx Vy Vz to Vx_tmp, dimension ntot1 = lx1*ly1*lz1*nelt 
      call opcopy(Vx_tilde, Vy_tilde, Vz_tilde, Vx, Vy, Vz) ! save intermediate velocity
      call opcopy(Vx, Vy, Vz, Vx_tmp, Vy_tmp, Vz_tmp) !restore previous step

      ! Save Vxlag, note dimension: vxlag (lx1,ly1,lz1,nelt,2) 
      DO ILAG=1,2,-1
       CALL COPY (VXLAG (1,1,1,1,ILAG),VXLAG_tmp (1,1,1,1,ILAG),NTOT1)
       CALL COPY (VYLAG (1,1,1,1,ILAG),VYLAG_tmp (1,1,1,1,ILAG),NTOT1)
        IF (ldim.EQ.3)
     $ CALL COPY (VZLAG (1,1,1,1,ILAG),VZLAG_tmp (1,1,1,1,ILAG),NTOT1)
      ENDDO

      ! save pr prlag, dimension: p(lx2,ly2,lz2,1)
      ntot2 = lx2*ly2*lz2*nelv
      do i=1,ntot2
         pr    (i,1,1,1)            = pr_tmp    (i,1,1,1)
         prlag (i,1,1,1,1)          = prlag_tmp (i,1,1,1,1)
      enddo

      ! 
      do i =1,ntot1
         abx2 (i,1,1,1) = abx2_tmp(i,1,1,1)
         aby2 (i,1,1,1) = aby2_tmp (i,1,1,1)
         abz2 (i,1,1,1) = abz2_tmp (i,1,1,1)
         abx1 (i,1,1,1) = abx1_tmp (i,1,1,1)
         aby1 (i,1,1,1) = aby1_tmp (i,1,1,1)
         abz1 (i,1,1,1) = abz1_tmp (i,1,1,1)
      enddo

      do i = 1,ntot1*(lorder-1)
         bm1lag(i,1,1,1,1) = bm1lag_tmp(i,1,1,1,1)       !!! mass matrix
      enddo

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      
c     method 2
c      call plan3

      ! method 3
c      CALL MAKEF ! RHS force
c      INTYPE = -1 ! what is this mean?
c      CALL SETHLM  (H1,H2,INTYPE) !  => H1 H2
c      CALL CRESVIF_ibm (RESV1,RESV2,RESV3,H1,H2) ! => RESV1,RESV2,RESV3
c      CALL OPHINV  (DV1,DV2,DV3,RESV1,RESV2,RESV3,H1,H2,TOLHV,NMXH) !solver
c      CALL OPADD2  (VX,VY,VZ,DV1,DV2,DV3)

      ibm_force_gate = 0 ! apply force
      
      return
      end
      
C---------------------------------------------------------------------
      subroutine cresvif_ibm (resv1,resv2,resv3,h1,h2)
C---------------------------------------------------------------------
C
C     Compute startresidual/right-hand-side in the velocity solver
C
C---------------------------------------------------------------------
      include 'SIZE'
      include 'TOTAL'
      REAL           RESV1 (LX1,LY1,LZ1,1)
      REAL           RESV2 (LX1,LY1,LZ1,1)
      REAL           RESV3 (LX1,LY1,LZ1,1)
      REAL           H1    (LX1,LY1,LZ1,1)
      REAL           H2    (LX1,LY1,LZ1,1)
      COMMON /SCRUZ/ W1    (LX1,LY1,LZ1,LELV)
     $ ,             W2    (LX1,LY1,LZ1,LELV)
     $ ,             W3    (LX1,LY1,LZ1,LELV)

      common /cgeom/ igeom

      write(6,*)"Call cresvif_ibm"
      NTOT1 = lx1*ly1*lz1*NELV
      NTOT2 = lx2*ly2*lz2*NELV
c      if (igeom.eq.2) CALL LAGVEL ! update vxlag lag velocity for rhs bdf abf
c      CALL BCDIRVC (VX,VY,VZ,v1mask,v2mask,v3mask) !Apply Dirichlet boundary conditions to surface of vector (V1,V2,V3).
c      CALL BCNEUTR  !? some surface boundary condition
C
c      call extrapp (pr,prlag) !Pressure extrapolation, 
c      call opgradt (resv1,resv2,resv3,pr) !? Compute DTx, DTy, DTz of an input field INPFLD: pr

      CALL OPADD2  (RESV1,RESV2,RESV3,BFX,BFY,BFZ) ! add resv1 = resv1 + bfx 
      CALL OPHX    (W1,W2,W3,VX,VY,VZ,H1,H2) ! W1 = (H1*A+H2*B) * INP, ?
      CALL OPSUB2  (RESV1,RESV2,RESV3,W1,W2,W3) ! resv1 = resv1 - w1
C
      RETURN
      END
c-----------------------------------------------------------------------


c---------------------------------------------------------------------	
      subroutine divergence_vel
c---------------------------------------------------------------------
c     compute divergence of velocity for check
c     Output: divergence

        include 'SIZE'
	include 'TOTAL'
	include 'LPM'

        parameter(lt=lx1*ly1*lz1*lelt)

	real divel (lt)
        real vort(lt,3),w1(lt),w2(lt)
        real l2(lt)
        
        call comp_vort3(vort,w1,w2,vx,vy,vz)
        call lambda2(l2)

        itmp = 1
        call outpost2(vort(1,1),  ! dvx/dx
     >                vort(1,2),  ! dvy/dy
     >                vort(1,3),  ! dvz/dz
     >                l2(1),      ! div(V)
     >                l2(1),      ! div(V)
     >                itmp          ,        
     >              'vort')

        call rzero(divel,lt)
        call gradm1(dvxdx,dvxdy,dvxdz,vx) ! compute grad of dudx, dudy, dudz 
        call gradm1(dvydx,dvydy,dvydz,vy) ! compute grad of dvdx, dvdy, dvdz 
        call gradm1(dvzdx,dvzdy,dvzdz,vz) ! compute grad of dwdx, dwdy, dwdz  
	call add4  (divel, dvxdx, dvydy, dvzdz, lt)

        call outpost2(dvxdx(1,1,1,1),  ! dvx/dx
     >                dvydy(1,1,1,1),  ! dvy/dy
     >                dvzdz(1,1,1,1),  ! dvz/dz
     >                divel(1),  ! div(V)
     >                l2(1),  ! div(V)
     >                itmp          ,        
     >              'div')

      return 
      end subroutine divergence_vel

c automatically added by makenek
      subroutine usrsetvert(glo_num,nel,nx,ny,nz) ! to modify glo_num
      integer*8 glo_num(1)

      return
      end

c automatically added by makenek
      subroutine userqtl

      call userqtl_scig

      return
      end
