#!/bin/bash

# Check if Jupyter is running, start it if not
if ! pgrep -f "jupyter-notebook" > /dev/null; then
    echo "Launching Ascent + Jupyter"
    nohup jupyter notebook --ip 0.0.0.0 --port 8888 --IdentityProvider.token='' --no-browser --NotebookApp.token='' --NotebookApp.password='' >/dev/null 2>&1 &
fi
