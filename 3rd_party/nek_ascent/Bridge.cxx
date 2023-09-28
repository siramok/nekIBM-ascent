#include "Bridge.h"
#include <unistd.h>

void ascent_setup(MPI_Comm *comm)
{
    conduit::Node ascent_opts;
    ascent_opts["mpi_comm"] = *comm;
    mAscent.open(ascent_opts);
}

void ascent_update(int *step, double *time, char *casename, int *elems,
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
    int n_elems = *elems;
    int x_dim = *vx_dim;
    int y_dim = *vy_dim;
    int z_dim = *vz_dim;
    int x_len = x_dim - 1;
    int y_len = y_dim - 1;
    int z_len = z_dim - 1;
    int mesh_size = x_dim * y_dim * z_dim * n_elems;
    int n_cells = 0;
    int conn_size = 0;

    if (z_dim == 1)
    {
        n_cells = x_len * y_len * n_elems;
        conn_size = n_cells * 4;
    }
    else
    {
        n_cells = x_len * y_len * z_len * n_elems;
        conn_size = n_cells * 8;
    }

    static std::vector<long> a_etov(conn_size);
    auto it = a_etov.begin();

    // Compute connectivity
    if (z_dim == 1)
    {
        for (int elem = 0; elem < n_elems; elem++)
        {
            for (int y = 0; y < y_len; y++)
            {
                for (int x = 0; x < x_len; x++)
                {

                    it[0] = x + x_dim * (y + y_dim * elem);
                    it[1] = it[0] + x_dim;
                    it[2] = it[1] + 1;
                    it[3] = it[0] + 1;
                    it += 4;
                }
            }
        }
    }
    else
    {
        for (int elem = 0; elem < n_elems; elem++)
        {
            for (int z = 0; z < z_len; z++)
            {
                for (int y = 0; y < y_len; y++)
                {
                    for (int x = 0; x < x_len; x++)
                    {

                        // Needs to be tested, probably needs fixing
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
    }

    // Prepare data
    conduit::Node data;
    data["state/cycle"] = *step;
    data["state/time"] = *time;

    data["coordsets/coords/type"] = "explicit";
    data["coordsets/coords/values/x"].set_external(vmesh_x, mesh_size);
    data["coordsets/coords/values/y"].set_external(vmesh_y, mesh_size);
    data["coordsets/coords/values/z"].set_external(vmesh_z, mesh_size);

    data["topologies/topo/type"] = "unstructured";
    data["topologies/topo/coordset"] = "coords";
    data["topologies/topo/elements/shape"] = "quad";
    data["topologies/topo/elements/connectivity"].set_external(a_etov);

    data["fields/vel_x/association"] = "vertex";
    data["fields/vel_x/topology"] = "topo";
    data["fields/vel_x/values"].set_external(vel_x, mesh_size);

    data["fields/vel_y/association"] = "vertex";
    data["fields/vel_y/topology"] = "topo";
    data["fields/vel_y/values"].set_external(vel_y, mesh_size);

    data["fields/vel_z/association"] = "vertex";
    data["fields/vel_z/topology"] = "topo";
    data["fields/vel_z/values"].set_external(vel_z, mesh_size);

    mAscent.publish(data);

    conduit::Node actions;
    mAscent.execute(actions);

    // For debug
    // int rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // if (*step == 20 && rank == 0)
    // {
    //     data.print();
    //     std::cin.get();
    // }
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
