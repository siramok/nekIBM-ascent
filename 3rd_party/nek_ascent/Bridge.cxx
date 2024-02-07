#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
#include <optional>
#include <unistd.h>
#include <vector>

#include "Bridge.h"

void ascent_setup(MPI_Comm *comm)
{
    conduit::Node ascent_opts;
    ascent_opts["mpi_comm"] = *comm;
    mAscent.open(ascent_opts);

    register_void_callback("get_dt", get_dt);
    register_void_callback("increase_dt", increase_dt);
    register_void_callback("decrease_dt", decrease_dt);
    register_void_callback("reduce_particles", reduce_particles);
}

void ascent_update(int *istep, double *time, int* ndim, int *nelt, int *nelv, int *n, int *lr, int *wdsize,
                       int *lx1, int *ly1, int *lz1, double *xm1, double *ym1, double *zm1,
                       int *lx2, int *ly2, int *lz2, double *xm2, double *ym2, double *zm2,
                       double *vx, double *vy, double *vz,
                       int *jx, int *jy, int *jz, int *jv0, double *rpart)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0)
    {
        std::cout << "Start of Ascent Update" << std::endl;
    }

    int dimensions = *ndim;
    int t_mesh_size = *nelt;
    int v_mesh_size = *nelv;

    int velocity_x_size = *lx1;
    int velocity_y_size = *ly1;
    int velocity_z_size = *lz1;

    int velocity_mesh_size = velocity_x_size;
    int velocity_array_size = velocity_x_size;

    if (velocity_y_size > 0) {
        velocity_mesh_size *= velocity_z_size;
        velocity_array_size *= velocity_y_size;
    }

    if (velocity_z_size > 0) {
        velocity_mesh_size *= velocity_z_size;
        velocity_array_size *= velocity_z_size;
    }

    if (t_mesh_size > 0) {
        velocity_mesh_size *= t_mesh_size;
    }

    if (v_mesh_size > 0) {
        velocity_array_size *= v_mesh_size;
    }
    
    int pressure_x_size = *lx2;
    int pressure_y_size = *ly2;
    int pressure_z_size = *lz2;
    int pressure_mesh_size = pressure_x_size * pressure_y_size * pressure_z_size * v_mesh_size;

    int x_len = (velocity_x_size - 1);
    int y_len = (velocity_y_size - 1);
    int z_len = (velocity_z_size - 1);
    int n_cells = x_len * y_len * z_len * t_mesh_size;

    static std::vector<long> conn(n_cells * 8);
    auto it = conn.begin();

    for (int elem = 0; elem < t_mesh_size; elem++)
    {
        for (int z = 0; z < z_len; z++)
        {
            for (int y = 0; y < y_len; y++)
            {
                for (int x = 0; x < x_len; x++)
                {
                    it[0] = x + velocity_x_size * (y + velocity_y_size * (z + velocity_z_size * elem));
                    it[1] = it[0] + 1;
                    it[2] = it[0] + velocity_x_size + 1;
                    it[3] = it[0] + velocity_x_size;
                    it[4] = it[0] + velocity_x_size * velocity_x_size;
                    it[5] = it[1] + velocity_x_size * velocity_x_size;
                    it[6] = it[2] + velocity_x_size * velocity_x_size;
                    it[7] = it[3] + velocity_x_size * velocity_x_size;
                    it += 8;
                }
            }
        }
    }

    // Prepare data
    conduit::Node data;
    data["state/cycle"] = *istep;
    data["state/time"] = *time;

    data["coordsets/coords/type"] = "explicit";
    data["coordsets/coords/values/x"].set_external(xm1, velocity_mesh_size);
    data["coordsets/coords/values/y"].set_external(ym1, velocity_mesh_size);
    data["coordsets/coords/values/z"].set_external(zm1, velocity_mesh_size);

    data["topologies/mesh/type"] = "unstructured";
    data["topologies/mesh/coordset"] = "coords";
    data["topologies/mesh/elements/shape"] = "hex";
    data["topologies/mesh/elements/connectivity"].set_external(conn);

    data["fields/height/association"] = "vertex";
    data["fields/height/topology"] = "mesh";
    data["fields/height/values"].set_external(zm1, velocity_mesh_size);

    data["fields/velocity/association"] = "vertex";
    data["fields/velocity/topology"] = "mesh";
    data["fields/velocity/values/u"].set_external(vx, velocity_array_size);
    data["fields/velocity/values/v"].set_external(vy, velocity_array_size);
    data["fields/velocity/values/w"].set_external(vz, velocity_array_size);


    if (rank == 0)
    {
        // std::cout << "dimensions: " << dimensions << std::endl;
        // std::cout << "t_mesh_size: " << t_mesh_size << std::endl;
        // std::cout << "v_mesh_size: " << v_mesh_size << std::endl;
        // std::cout << "velocity_x_size: " << velocity_x_size << std::endl;
        // std::cout << "velocity_y_size: " << velocity_y_size << std::endl;
        // std::cout << "velocity_z_size: " << velocity_z_size << std::endl;
        // std::cout << "pressure_x_size: " << pressure_x_size << std::endl;
        // std::cout << "pressure_y_size: " << pressure_y_size << std::endl;
        // std::cout << "pressure_z_size: " << pressure_z_size << std::endl;
        // std::cout << "velocity_mesh_size: " << velocity_mesh_size << std::endl;
        // std::cout << "velocity_array_size: " << velocity_array_size << std::endl;
        // std::cout << "pressure_mesh_size: " << pressure_mesh_size << std::endl;

        conduit::Node verify_info;
        if(!conduit::blueprint::mesh::verify(data,verify_info))
        {
            CONDUIT_INFO("blueprint verify failed!" + verify_info.to_yaml());
        }
    }

    mAscent.publish(data);

    conduit::Node actions;
    mAscent.execute(actions);

    if (rank == 0)
    {
        std::cout << "Invoking get_dt" << std::endl;
        conduit::Node params;
        conduit::Node output;
        get_dt(params, output);
    }

    // if (*istep == 3) {
    //     conduit::Node params;
    //     conduit::Node output;
    //     for (int i = 0; i < 95; i++) {
    //         decrease_dt(params, output);
    //     }
    // }

    if (rank == 0) {
        std::cout << "End of Ascent Update" << std::endl;
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
extern "C" void nek_ascent_get_dt_();
extern "C" void nek_ascent_increase_dt_();
extern "C" void nek_ascent_decrease_dt_();

void get_dt(conduit::Node &params, conduit::Node &output)
{
    nek_ascent_get_dt_();
}

void increase_dt(conduit::Node &params, conduit::Node &output)
{
    nek_ascent_increase_dt_();
}

void decrease_dt(conduit::Node &params, conduit::Node &output)
{
    nek_ascent_decrease_dt_();
}

void reduce_particles(conduit::Node &params, conduit::Node &output)
{
    const std::string &inputFilename = params["inputFilename"].as_string();
    const std::string &outputFilename = params["outputFilename"].as_string();
    float removalPercentage = params["removalPercentage"].to_uint8();
    int seed = 0;
    // if (params["seed"].has_path())
    // {
    //     seed = params["seed"].value();
    // } else {
        
    // }

    if (outputFilename == "particles.dat") {
        std::cerr << "Error: Output filename cannot be 'particles.dat' to protect the original data file." << std::endl;
        return;
    }

    std::ifstream inputFile(inputFilename);
    std::ofstream outputFile(outputFilename);
    std::string line;
    std::vector<std::string> lines;
    int particleCount = 0;

    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << inputFilename << std::endl;
        return;
    }

    // Read the header
    getline(inputFile, line);
    std::stringstream ss(line);
    ss >> particleCount;  // Assuming the first number is the particle count
    lines.push_back(line); // Add header to lines

    // Read particle data
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    // Initialize random number generator
    std::mt19937 gen;
    gen.seed(seed);
    // if (seed) {
        
    // } else {
    //     std::random_device rd;
    //     gen.seed(rd());
    // }
    std::uniform_real_distribution<> dis(0, 1);

    // Randomly remove lines
    for (int i = 1; i < lines.size(); ) {
        if (dis(gen) < removalPercentage) {
            lines.erase(lines.begin() + i);
            particleCount--;
        } else {
            ++i;
        }
    }

    // Renumber particle identifiers
    for (int i = 1; i <= particleCount; ++i) {
        std::stringstream lineStream(lines[i]);
        std::string particleData;
        getline(lineStream, particleData, '\t'); // Skip old number
        getline(lineStream, particleData); // Get the rest of the line
        lines[i] = std::to_string(i) + '\t' + particleData;
    }

    // Update header with new particle count and first particle data (if available)
    if (lines.size() > 1) {
        std::stringstream firstParticleStream(lines[1]);
        std::string firstParticleData;
        getline(firstParticleStream, firstParticleData, '\t'); // Skip old number
        getline(firstParticleStream, firstParticleData); // Get the rest of the line

        std::stringstream newHeader;
        newHeader << particleCount << '\t' << firstParticleData;
        lines[0] = newHeader.str();
    }

    // Write to new file
    for (const auto& l : lines) {
        outputFile << l << "\n";
    }
    outputFile.close();
}