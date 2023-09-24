#ifndef NEK5000_BRIDGE_H
#define NEK5000_BRIDGE_H

#include <ascent.hpp>
#include <conduit_blueprint.hpp>
#include <iostream>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef UPCASE
#define FORTRAN_NAME(low, up) up
#else
#ifdef UNDERSCORE
#define FORTRAN_NAME(low, up) low##_
#else
#define FORTRAN_NAME(low, up) low
#endif
#endif

#define ascent_setup FORTRAN_NAME(ascent_setup, ASCENT_SETUP)
#define ascent_update FORTRAN_NAME(ascent_update, ASCENT_UPDATE)
#define ascent_finalize FORTRAN_NAME(ascent_finalize, ASCENT_FINALIZE)

    void ascent_setup();
    void ascent_update();
    void ascent_finalize();

#ifdef __cplusplus
} // extern "C"
#endif

#endif