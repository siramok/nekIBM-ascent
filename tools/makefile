SHELL = /bin/bash

all:
	@for i in $(MODULES) ; do                \
	  if [ "$$i" == "amg_matlab2" ]; then    \
             continue;                           \
     fi ;                                   \
     if [ "$$i" == "postnek" ]; then    \
          continue;                           \
     fi ;                                   \
	  if test ! -f $${i}/makefile ; then     \
           echo "" ;                             \
           echo "ERROR: $${i} does not exist!" ; \
           echo "" ;                             \
            exit 1 ;                             \
     fi ;                                   \
	  export FFLAGS="${NEK_FFLAGS}" ;        \
          export FFLAGS+=" ${CPPF}" ;            \
          export FFLAGS+=" ${BIGMEM}" ;          \
	  export CFLAGS="${NEK_CFLAGS}" ;        \
          export CFLAGS+=" ${US}" ;              \
          export CFLAGS+=" ${BIGMEM}" ;          \
          if [ "$$i" == "exo2nek" ]; then        \
             export FFLAGS+=" ${R8}" ;           \
             if [ ! -z $(DOALL) ]; then          \
                continue;                        \
             fi ;                                \
          fi ;                                   \
          if [ "$$i" == "genmap" ]; then         \
             export FFLAGS+=" ${R8}" ;           \
          fi ;                                   \
          if [ "$$i" == "genbox" ]; then         \
            export FFLAGS+=" ${R8}" ;            \
	  fi ;                                   \
          if [ "$$i" == "prenek" ]; then         \
             export CFLAGS+=" -Dr8";             \
          fi ;                                   \
          echo "----------------------";         \
          echo "Make $${i}..." ;                 \
          echo "----------------------";         \
          cd $${i} ;                             \
          ${MAKE} ;                              \
          if test  $$? -ne 0 ; then              \
            echo "" ;                            \
            echo "ERROR: $${i} failed to compile!" ; \
            exit 1 ;                             \
          fi ;                                   \
          cd .. ;                                \
          echo "" ;                              \
	  done ;

clean:
	@for i in $(MODULES) ; do           \
          if test -f $${i}/makefile ;  then \
            cd $${i} ;                      \
            ${MAKE} clean ;                 \
            cd .. ;                         \
          fi ;                              \
        done ;
