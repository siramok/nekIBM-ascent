#include "Bridge.h"
#include <unistd.h>

void ascent_setup(MPI_Comm *comm, char *casename, int *elems,
                  int *vx_dim, int *vy_dim, int *vz_dim, double *vmesh_x, double *vmesh_y, double *vmesh_z,
                  int *px_dim, int *py_dim, int *pz_dim, double *pmesh_x, double *pmesh_y, double *pmesh_z,
                  double *vel_x, double *vel_y, double *vel_z,
                  double *vort_x, double *vort_y, double *vort_z,
                  double *pressure, double *temp, double *jacobian, int *t_dim,
                  double *x_min, double *x_max, double *y_min, double *y_max, double *z_min, double *z_max,
                  double *vel_x_min, double *vel_x_max, double *vel_y_min, double *vel_y_max, double *vel_z_min, double *vel_z_max,
                  double *vort_x_min, double *vort_x_max, double *vort_y_min, double *vort_y_max, double *vort_z_min, double *vort_z_max,
                  double *pr_min, double *pr_max, double *temp_min, double *temp_max, double *jac_min, double *jac_max)
{
    conduit::Node ascent_opts;
    ascent_opts["mpi_comm"] = *comm;
    mAscent.open(ascent_opts);

    int x_dim = *vx_dim;
    int y_dim = *vy_dim;
    int z_dim = *vz_dim;
    int x_len = x_dim - 1;
    int y_len = y_dim - 1;
    int z_len = z_dim - 1;
    int n_elems = *elems;
    int n_cells = x_len * y_len * z_len * n_elems;

    std::vector<int> a_etov(n_cells * 8);
    auto it = a_etov.begin();

    // TODO: fix
    for (int elem = 0; elem < n_elems; elem++)
    {
        for (int z = 0; z < z_len; z++)
        {
            for (int y = 0; y < y_len; y++)
            {
                for (int x = 0; x < x_len; x++)
                {

                    it[0] = x + x_dim * (y + y_dim * (z + z_dim * elem));
                    it[1] = it[0] + x_dim * y_dim;
                    it[2] = it[1] + x_dim;
                    it[3] = it[0] + x_dim;
                    it[4] = it[0] + 1;
                    it[5] = it[1] + 1;
                    it[6] = it[2] + 1;
                    it[7] = it[3] + 1;
                    it += 8;
                }
            }
        }
    }

    // Prepare data
    data["coordsets/coords/type"] = "explicit";
    data["coordsets/coords/values/x"].set_external(vmesh_x, *elems);
    data["coordsets/coords/values/y"].set_external(vmesh_y, *elems);
    data["coordsets/coords/values/z"].set_external(vmesh_z, *elems);

    data["topologies/topo/type"] = "unstructured";
    data["topologies/topo/coordset"] = "coords";
    data["topologies/topo/elements/shape"] = "hex";
    data["topologies/topo/elements/connectivity"].set_external(a_etov);

    data["fields/vel_x/association"] = "vertex";
    data["fields/vel_x/topology"] = "topo";
    data["fields/vel_x/values"].set_external(vel_x, *elems);

    data["fields/vel_y/association"] = "vertex";
    data["fields/vel_y/topology"] = "topo";
    data["fields/vel_y/values"].set_external(vel_y, *elems);

    data["fields/vel_z/association"] = "vertex";
    data["fields/vel_z/topology"] = "topo";
    data["fields/vel_z/values"].set_external(vel_z, *elems);
}

void ascent_update(int *step, double *time)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    data["state/cycle"] = *step;
    data["state/time"] = *time;

    mAscent.publish(data);

    conduit::Node actions;
    mAscent.execute(actions);

    if (*step % 10 == 0)
    {
        if (rank == 0)
        {
            data.print();
        }

        std::cin.get();
    }
}

void ascent_finalize()
{
    mAscent.close();
}

// Callback registration
void register_void_callback(std::string callback_name, void (*callback_function)(conduit::Node &, conduit::Node &))
{
    ascent::register_callback(callback_name, callback_function);
}

void register_bool_callback(std::string callback_name, bool (*callback_function)(void))
{
    ascent::register_callback(callback_name, callback_function);
}

// Callbacks
