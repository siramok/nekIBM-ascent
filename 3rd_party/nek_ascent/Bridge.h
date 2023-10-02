#ifndef NEK5000_BRIDGE_H
#define NEK5000_BRIDGE_H

#include <ascent.hpp>
#include <conduit_blueprint.hpp>
#include <iostream>
#include <mpi.h>
#include <vector>

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

    static ascent::Ascent mAscent;

    // Accessible to Fortran
    void ascent_setup(MPI_Comm *comm);
    void ascent_update(int *tstep, double *time, char *casename, int *elems,
                       int *vx_dim, int *vy_dim, int *vz_dim, double *vmesh_x, double *vmesh_y, double *vmesh_z,
                       int *px_dim, int *py_dim, int *pz_dim, double *pmesh_x, double *pmesh_y, double *pmesh_z,
                       double *vel_x, double *vel_y, double *vel_z,
                       double *vort_x, double *vort_y, double *vort_z,
                       double *pressure, double *temp, double *jacobian, int *t_dim,
                       double *x_min, double *x_max, double *y_min, double *y_max, double *z_min, double *z_max,
                       double *vel_x_min, double *vel_x_max, double *vel_y_min, double *vel_y_max, double *vel_z_min, double *vel_z_max,
                       double *vort_x_min, double *vort_x_max, double *vort_y_min, double *vort_y_max, double *vort_z_min, double *vort_z_max,
                       double *pr_min, double *pr_max, double *temp_min, double *temp_max, double *jac_min, double *jac_max);
    void ascent_finalize();

    // Callback registration
    void register_void_callback(std::string callback_name, void (*callback_function)(conduit::Node &, conduit::Node &));
    void register_bool_callback(std::string callback_name, bool (*callback_function)(void));

    // Callbacks
    void callback_test(conduit::Node &params, conduit::Node &output);

#ifdef __cplusplus
} // extern "C"
#endif

#endif