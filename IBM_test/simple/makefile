### makefile automatically created by makenek 01/07/2020 02:16:08 ###
BINNAME=nek5000
LIBNAME=libnek5000.a
CASENAME:=uniform
CASEDIR:=/ufrc/bala/hityangsir/Nek5000-v1-6-4-Spheroid-Periodic/Github/checkout/NekIBM/IBM_test/simple
S:=../../../NekIBM/core
SCMT:=../../../NekIBM/core/cmt
OPT_INCDIR=./
OBJDIR=obj
MPI:=1
CMT:=0
LPM:=1
NEKNEK:=0
FC:=mpif77 -mcmodel=large
CC:=mpicc
FFLAGS:= -r8 -fpconstant -fpp
CFLAGS:=
PPPO=
PPS=LPM MPI UNDERSCORE TIMER
USR:=
USR_LFLAGS:= -L../../../NekIBM/3rd_party/gslib/lib -lgs
VISIT:=0
VISIT_IFLAGS=
VISIT_LFLAGS=

################################################################################

PPS_F = $(patsubst %,$(PPPO)-D%,$(PPS))
PPS_C = $(patsubst %,-D%,$(PPS))

# CORE##########################################################################
CORE = drive.o drive1.o drive2.o \
plan5.o \
plan4.o bdry.o coef.o conduct.o connect1.o connect2.o \
dssum.o edgec.o eigsolv.o gauss.o genxyz.o navier1.o makeq.o \
navier0.o navier2.o navier3.o navier4.o prepost.o speclib.o \
map2.o mvmesh.o ic.o gfldr.o ssolv.o planx.o math.o mxm_wrapper.o \
hmholtz.o gfdm_par.o  gfdm_op.o gfdm_solve.o subs1.o subs2.o \
genbox.o gmres.o hsmg.o convect.o induct.o perturb.o \
navier5.o navier6.o navier7.o navier8.o fast3d.o fasts.o calcz.o \
byte.o chelpers.o byte_mpi.o postpro.o intp_usr.o \
cvode_driver.o nek_comm.o \
vprops.o makeq_aux.o \
papi.o nek_in_situ.o \
reader_rea.o reader_par.o reader_re2.o \
finiparser.o iniparser.o dictionary.o \
hpf.o
################################################################################
# MXM 
MXM=mxm_std.o
################################################################################
# BLAS
BLAS=blas.o dsygv.o

# YP NEKNEK SOLVER
ifeq ($(NEKNEK),0)
NEKNEKO := singlmesh.o
else
NEKNEKO := multimesh.o
endif

COMM_MPI := comm_mpi.o
ifeq ($(MPI),0)
  COMM_MPI := ${COMM_MPI} mpi_dummy.o
endif

# CMT append *.o files to CORE
ifeq ($(CMT),0)
  CMTCORE := 
else
  CMTCORE := drive1_cmt.o drive2_cmt.o driver3_cmt.o \
   face.o ausm.o  MixtPerf.o intpdiff.o diffusive_cmt.o eqnsolver_cmt.o surface_fluxes.o \
   bc.o inflow_bc.o outflow_bc.o wall_bc.o step.o \
   filters_cmt.o diagnostics.o artvisc.o
endif
# VISIT ########################################################################
ifneq ($(VISIT),0)
  VISITNEK_INCLUDES:=-I$S/3rd_party
  VISITO := visit.o 
  USR_LFLAGS := ${USR_LFLAGS} $(VISIT_LFLAGS)
endif

ifeq ($(MPI),0)
	DUMMY:= $(shell cp $S/mpi_dummy.h $S/mpif.h) 
else
	DUMMY:= $(shell rm -rf $S/mpif.h) 
endif

TMP1 = $(CORE) $(MXM) $(USR) $(COMM_MPI) $(NEKNEKO) $(VISITO) $(BLAS)

ifeq ($(CMT),1)
TMP1 += $(CMTCORE)
OPT_INCDIR += -I $(SCMT)
endif
ifeq ($(LPM),1)
  TMP1 += lpm.o 
endif

NOBJS = $(patsubst %,$(OBJDIR)/%,$(TMP1))

L0 = $(G) -O0
L2 = $(G) -O2
L3 = $(G) -O3

FL0   = $(L0) $(FFLAGS) $(PPS_F) -I$(CASEDIR) -I$S -I$(OPT_INCDIR) 
FL2   = $(L2) $(FFLAGS) $(PPS_F) -I$(CASEDIR) -I$S -I$(OPT_INCDIR)
FL3   = $(L3) $(FFLAGS) $(PPS_F) -I$(CASEDIR) -I$S -I$(OPT_INCDIR)

cFL0   = $(L0) $(CFLAGS) $(PPS_C)
cFL2   = $(L2) $(CFLAGS) $(PPS_C)
cFL3   = $(L3) $(CFLAGS) $(PPS_C)

################################################################################
all : nek5000

objdir: 
	@mkdir $(OBJDIR) 2>/dev/null; cat /dev/null 

usrfile:
	@env CASENAME=$(CASENAME) PPS="$(PPS)" $S/mkuserfile

nek5000:	objdir $(NOBJS) usrfile
	$(FC) -c $(FL2) $(CASEDIR)/${CASENAME}.f $(VISITNEK_INCLUDES) -o ${OBJDIR}/${CASENAME}.o 
	$(FC) $(FL2) -o ${BINNAME} ${OBJDIR}/${CASENAME}.o $(NOBJS) $(USR_LFLAGS) $(LDFLAGS)
	@if test -f ${BINNAME}; then \
        echo ""; \
	echo "#############################################################"; \
	echo "#                  Compilation successful!                  #"; \
	echo "#############################################################"; \
	size ${BINNAME}; \
	echo ""; \
        echo "$(NEK_WARN)"; \
	else \
	echo -e "\033[1;31;38m" "ERROR: Compilation failed!"; \
	echo -e "\033[0m"; \
	fi
ifeq ($(MPI),0) 
	@rm -rf $S/mpif.h
endif

lib:	objdir $(NOBJS)
	$(AR) cru ${LIBNAME} $(NOBJS)
	ranlib ${LIBNAME}

clean:
	@echo "cleaning Nek5000 ..."
	@rm -rf ${CASENAME}.f obj ${BINNAME}
ifeq ($(MPI),0) 
	@rm -rf $S/mpif.h
endif

$(NOBJS) : ./SIZE

# NEK CORE     ##################################################################
$(OBJDIR)/drive.o	:$S/drive.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/drive1.o	:$S/drive1.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/drive2.o	:$S/drive2.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/prepost.o	:$S/prepost.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/postpro.o	:$S/postpro.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/intp_usr.o	:$S/intp_usr.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gfldr.o	:$S/gfldr.f $S/GFLDR;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/connect1.o	:$S/connect1.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/connect2.o	:$S/connect2.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/edgec.o	:$S/edgec.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/genxyz.o	:$S/genxyz.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/subs1.o	:$S/subs1.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/subs2.o	:$S/subs2.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/map2.o	:$S/map2.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/mvmesh.o	:$S/mvmesh.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/induct.o	:$S/induct.f;	      		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/convect.o	:$S/convect.f;	      		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/perturb.o	:$S/perturb.f;	      		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/genbox.o	:$S/genbox.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/hsmg.o	:$S/hsmg.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gmres.o	:$S/gmres.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/papi.o	:$S/papi.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/bdry.o	:$S/bdry.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/ic.o		:$S/ic.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/conduct.o	:$S/conduct.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier0.o	:$S/navier0.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier2.o	:$S/navier2.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier3.o	:$S/navier3.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier4.o	:$S/navier4.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier5.o	:$S/navier5.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier6.o	:$S/navier6.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier7.o	:$S/navier7.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier8.o	:$S/navier8.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/speclib.o	:$S/speclib.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/calcz.o	:$S/calcz.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/fast3d.o	:$S/fast3d.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/dssum.o	:$S/dssum.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/eigsolv.o	:$S/eigsolv.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gauss.o	:$S/gauss.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/planx.o	:$S/planx.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/ssolv.o	:$S/ssolv.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gfdm_par.o	:$S/gfdm_par.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gfdm_solve.o	:$S/gfdm_solve.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gfdm_op.o	:$S/gfdm_op.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/coef.o	:$S/coef.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/plan4.o	:$S/plan4.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/plan5.o	:$S/plan5.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/vprops.o	:$S/vprops.f;	   		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/cvode_driver.o :$S/cvode_driver.f $S/CVODE;	$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/makeq.o      	:$S/makeq.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/makeq_aux.o  	:$S/makeq_aux.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/hmholtz.o	:$S/hmholtz.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/navier1.o	:$S/navier1.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/fasts.o	:$S/fasts.f;		        $(FC) -c $(FL2) $< -o $@
$(OBJDIR)/comm_mpi.o	:$S/comm_mpi.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/mpi_dummy.o	:$S/mpi_dummy.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/byte_mpi.o	:$S/byte_mpi.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/math.o	:$S/math.f;			$(FC) -c $(FL3) $< -o $@
$(OBJDIR)/multimesh.o	:$S/multimesh.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/singlmesh.o	:$S/singlmesh.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/lb_setqvol.o	:$S/lb_setqvol.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/reader_rea.o	:$S/reader_rea.f;	 	$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/reader_par.o	:$S/reader_par.f $S/PARDICT;	$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/reader_re2.o	:$S/reader_re2.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/hpf.o	       	:$S/hpf.f;         		$(F77) -c $(FL2) $< -o $@

# CMT CORE     ##################################################################
$(OBJDIR)/step.o	:$(SCMT)/step.f;        	$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/drive1_cmt.o	:$(SCMT)/drive1_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/drive2_cmt.o	:$(SCMT)/drive2_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/driver3_cmt.o	:$(SCMT)/driver3_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/gradient_solver.o	:$(SCMT)/gradient_solver.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/eqnsolver_cmt.o	:$(SCMT)/eqnsolver_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/ausm.o	:$(SCMT)/ausm.f;   		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/artvisc.o	:$(SCMT)/artvisc.f;   		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/face.o	:$(SCMT)/face.f;  		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/bc.o		:$(SCMT)/bc.f;			$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/surface_fluxes.o	:$(SCMT)/surface_fluxes.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/diffusive_cmt.o	:$(SCMT)/diffusive_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/MixtPerf.o	:$(SCMT)/MixtPerf.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/intpdiff.o	:$(SCMT)/intpdiff.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/inflow_bc.o	:$(SCMT)/inflow_bc.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/outflow_bc.o	:$(SCMT)/outflow_bc.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/wall_bc.o	:$(SCMT)/wall_bc.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/filters_cmt.o	:$(SCMT)/filters_cmt.f;		$(FC) -c $(FL2) $< -o $@
$(OBJDIR)/diagnostics.o	:$(SCMT)/diagnostics.f;		$(FC) -c $(FL2) $< -o $@

# LPM CORE     ##################################################################
$(OBJDIR)/lpm.o  	:$S/lpm.f $S/LPM;             	$(FC) -c $(FL2) $< -o $@

# MXM       ############################################################################
$(OBJDIR)/mxm_wrapper.o	  :$S/mxm_wrapper.f;		$(FC) -c $(FL2) $< -o $@ 
$(OBJDIR)/mxm_std.o	  :$S/mxm_std.f;		$(FC) -c $(FL3) $< -o $@
$(OBJDIR)/mxm_bgq.o	  :$S/mxm_bgq.f;		$(FC) -c $(FL3) $< -o $@

# C Files ##################################################################################
$(OBJDIR)/nek_comm.o             :$S/nek_comm.c;          $(CC) -c $(cFL2) $< -o $@
$(OBJDIR)/byte.o                 :$S/byte.c;              $(CC) -c $(cFL2) $< -o $@
$(OBJDIR)/chelpers.o             :$S/chelpers.c;          $(CC) -c $(cFL2) $< -o $@

# 3rd party #######################################################################################
$(OBJDIR)/dsygv.o     	:$S/3rd_party/dsygv.f;  	$(FC) -c $(L0) $(PPS_F) $< -o $@
$(OBJDIR)/blas.o   	:$S/3rd_party/blas.f;		$(FC) -c $(L0) $(PPS_F) $< -o $@
$(OBJDIR)/visit.o        :$S/3rd_party/visit.f;		$(FC) -c $(VISIT_IFLAGS) $(FL2) $< -o $@
$(OBJDIR)/nek_in_situ.o  :$S/3rd_party/nek_in_situ.f;	$(FC) -c $(VISIT_IFLAGS) $(FL2) $< -o $@
$(OBJDIR)/finiparser.o   :$S/3rd_party/finiparser.c;    $(CC)  -c $(cFL2) $(JL) $< -o $@
$(OBJDIR)/iniparser.o    :$S/3rd_party/iniparser.c;     $(CC)  -c $(cFL2) $(JL) $< -o $@
$(OBJDIR)/dictionary.o   :$S/3rd_party/dictionary.c;    $(CC)  -c $(cFL2) $(JL) $< -o $@
