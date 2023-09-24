#!/bin/bash
module swap PrgEnv-intel PrgEnv-gnu
./maketools clean
module swap PrgEnv-gnu PrgEnv-intel
