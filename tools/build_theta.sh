#!/bin/bash
module swap PrgEnv-intel PrgEnv-gnu
./maketools all
module swap PrgEnv-gnu PrgEnv-intel
