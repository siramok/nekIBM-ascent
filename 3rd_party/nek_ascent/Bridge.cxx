#include <cstdlib>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
#include <optional>
#include <unistd.h>
#include <vector>

#include "Bridge.h"

class SteeringInterface
{
public:
    SteeringInterface()
    {
        // Commands
        commands["exit"] = [this]()
        { this->exitShell(); };
        commands["help"] = [this]()
        { this->printHelp(); };
        commands["list"] = [this]()
        { this->listCallbacks(); };
        commands["param"] = [this]()
        { this->printParams(); };
        commands["run"] = [this]()
        { this->doNothing(); };

        // Descriptions
        descriptions["exit"] = "Exit the shell.";
        descriptions["help"] = "Print this help message.";
        descriptions["list"] = "List all registered Ascent callbacks.";
        descriptions["param"] = "Prints the current parameters. Can set, modify, or parameters with 'param add|edit|remove key value.";
        descriptions["run"] = "Run an Ascent callback with 'run my_callback'.";

        m_rank = 0;
    }

    SteeringInterface(int rank)
    {
        // Commands
        commands["exit"] = [this]()
        { this->exitShell(); };
        commands["help"] = [this]()
        { this->printHelp(); };
        commands["list"] = [this]()
        { this->listCallbacks(); };
        commands["param"] = [this]()
        { this->printParams(); };
        commands["run"] = [this]()
        { this->doNothing(); };

        // Descriptions
        descriptions["exit"] = "Exit the interactive steering interface.";
        descriptions["help"] = "Print this help message.";
        descriptions["list"] = "List all registered Ascent callbacks.";
        descriptions["param"] = "Prints the current parameters. Can set or modify parameters with 'param key value' or reset them with 'param reset'.";
        descriptions["run"] = "Run an Ascent callback with 'run my_callback'.";

        m_rank = rank;
    }

    void run()
    {
        if (m_rank == 0)
        {
            std::cout << "-------------Entering interactive steering mode-------------" << std::endl;
            std::cout << "Type 'help' to see available commands\n"
                      << std::endl;
        }

        std::string input;
        while (running)
        {
            if (m_rank == 0)
            {
                std::cout << "-> " << std::flush;
                std::getline(std::cin, input);
            }

            int input_size = input.size();
            MPI_Bcast(&input_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (m_rank != 0)
            {
                input.resize(input_size);
            }
            MPI_Bcast(const_cast<char *>(input.data()), input_size, MPI_CHAR, 0, MPI_COMM_WORLD);

            std::istringstream iss(input);
            std::string cmd;
            iss >> cmd;
            std::vector<std::string> tokens;
            std::string token;
            while (iss >> token)
            {
                tokens.push_back(token);
            }
            parseInput(cmd, tokens);
        }
        if (m_rank == 0)
        {
            std::cout << "\n-------------Exiting interactive steering mode-------------" << std::endl;
        }
    }

private:
    std::map<std::string, std::function<void()>> commands;
    std::map<std::string, std::string> descriptions;
    conduit::Node params;
    conduit::Node output;
    int m_rank;
    bool running = true;

    void doNothing()
    {
        if (m_rank == 0)
        {
            std::cout << std::endl;
        }
    }

    void exitShell()
    {
        running = false;
    }

    void parseInput(std::string cmd, std::vector<std::string> args)
    {
        if (commands.find(cmd) == commands.end())
        {
            if (m_rank == 0)
            {
                std::cout << "\nUnknown command: " << cmd << std::endl;
                printHelp();
            }
            return;
        }

        if (cmd == "run" && !args.empty())
        {
            runCallback(args);
        }
        else if (cmd == "param" && !args.empty())
        {
            modifyParams(args);
        }
        else
        {
            commands[cmd]();
        }
    }

    void listCallbacks()
    {
        if (m_rank == 0)
        {
            std::cout << "\nAvailable callbacks:" << std::endl;
            std::vector<std::string> callbacks = ascent::get_registered_callbacks();
            for (const auto &callback : callbacks)
            {
                std::cout << callback << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void modifyParams(std::vector<std::string> args)
    {
        std::string cmd;
        if (args.size() > 0)
        {
            cmd = args[0];
        }
        std::string arg;
        if (args.size() > 1)
        {
            arg = args[1];
        }

        if (cmd == "reset")
        {
            params.reset();
        }
        else if (cmd == "delete")
        {
            params[arg].reset();
        }
        else if (!arg.empty())
        {
            try
            {
                int possible_int = std::stoi(arg);
                params[cmd] = possible_int;
            }
            catch (const std::exception &e)
            {
            }
            try
            {
                float possible_float = std::stof(arg);
                params[cmd] = possible_float;
            }
            catch (const std::exception &e)
            {
            }
            try
            {
                double possible_double = std::stod(arg);
                params[cmd] = possible_double;
            }
            catch (const std::exception &e)
            {
                params[cmd] = arg;
            }
        }

        printParams();
    }

    void printHelp()
    {
        if (m_rank == 0)
        {
            std::cout << "\nAvailable commands:" << std::endl;
            for (const auto &cmd : descriptions)
            {
                std::cout << cmd.first << "\t-   " << cmd.second << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void printParams()
    {
        if (m_rank == 0)
        {
            std::cout << "\nCurrent params:";
            params.print();
            std::cout << std::endl;
        }
    }

    void runCallback(std::vector<std::string> args)
    {
        try
        {
            std::string callback = args[0];
            if (m_rank == 0)
            {
                std::cout << "\nRunning callback: " << callback << "\n"
                          << std::endl;
            }
            ascent::execute_callback(callback, params, output);
            MPI_Barrier(MPI_COMM_WORLD);
            if (m_rank == 0)
            {
                std::cout << "\n"
                          << callback << " output:";
                output.print();
                std::cout << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

void ascent_setup(MPI_Comm *comm)
{
    conduit::Node ascent_opts;
    ascent_opts["mpi_comm"] = *comm;
    mAscent.open(ascent_opts);

    register_void_callback("start_terminal_interface", start_terminal_interface);
    register_void_callback("get_dt", get_dt);
    register_void_callback("increase_dt", increase_dt);
    register_void_callback("decrease_dt", decrease_dt);
    register_void_callback("reduce_particles", reduce_particles);
    register_void_callback("load_new_data", load_new_data);
    register_void_callback("plot_bins", plot_bins);
}

void ascent_update(int *istep, double *time, int *ndim, int *nelt, int *nelv, int *n, int *lr, int *wdsize,
                   int *lx1, int *ly1, int *lz1, double *xm1, double *ym1, double *zm1,
                   int *lx2, int *ly2, int *lz2, double *xm2, double *ym2, double *zm2,
                   double *vx, double *vy, double *vz,
                   int *jx, int *jy, int *jz, int *jv0, double *rpart)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        std::cout << "-------------Start of Ascent Update-------------" << std::endl;
    }

    int dimensions = *ndim;
    int t_mesh_size = *nelt;
    int v_mesh_size = *nelv;

    int velocity_x_size = *lx1;
    int velocity_y_size = *ly1;
    int velocity_z_size = *lz1;

    int velocity_mesh_size = velocity_x_size;
    int velocity_array_size = velocity_x_size;

    if (velocity_y_size > 0)
    {
        velocity_mesh_size *= velocity_z_size;
        velocity_array_size *= velocity_y_size;
    }

    if (velocity_z_size > 0)
    {
        velocity_mesh_size *= velocity_z_size;
        velocity_array_size *= velocity_z_size;
    }

    if (t_mesh_size > 0)
    {
        velocity_mesh_size *= t_mesh_size;
    }

    if (v_mesh_size > 0)
    {
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

    conduit::Node verify_info;
    if (!conduit::blueprint::mesh::verify(data, verify_info))
    {
        if (rank == 0)
        {
            CONDUIT_INFO("blueprint verify failed!" + verify_info.to_yaml());
        }
    }
    else
    {
        mAscent.publish(data);
        conduit::Node actions;
        mAscent.execute(actions);
    }

    if (rank == 0)
    {
        std::cout << "-------------End of Ascent Update-------------" << std::endl;
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
extern "C" void nek_ascent_load_new_data_();

void start_terminal_interface(conduit::Node &params, conduit::Node &output)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    SteeringInterface steering(rank);
    steering.run();
}

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
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0)
    {
        return;
    }

    bool success = true;
    float voxel_size;

    if (!params.has_path("voxel_size"))
    {
        std::cout << "MissingParam: voxel_size is a required parameter but was not found" << std::endl;
        success = false;
    }
    else
    {
        try
        {
            voxel_size = params["voxel_size"].as_float64();
            if (voxel_size < 0 || voxel_size > 1)
            {
                std::cout << "ParamError: voxel_size must be a decimal number between 0 and 1" << std::endl;
                success = false;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "ParamError: voxel_size must be numeric" << std::endl;
            success = false;
        }
    }

    if (!success)
    {
        output["success"] = "no";
        return;
    }

    std::string command = "python generate_particles1.py " + std::to_string(voxel_size);
    std::system(command.c_str());

    output["success"] = "yes";
}

void plot_bins(conduit::Node &params, conduit::Node &output)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        system("python plot_bins.py");
    }
    output["success"] = "yes";
}

void load_new_data(conduit::Node &params, conduit::Node &output)
{
    nek_ascent_load_new_data_();
    output["success"] = "yes";
}
