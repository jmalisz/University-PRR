# Matrix inversion program (Gaussian elimination)

To use compile with `g++ !(openmpi).cpp -o main.exe -fopenmp -I .`

## Inputs:

- Accepts input from "Input.txt" file in current directory (first number in "Input.txt" is matrix size)
- Generates a random matrix with chosen size

To make file inputs easier there are a few matrices prepared.

## Outputs:

- Outputs to "Output.txt" file in current directory
- Console

## Algorithm types:

- Sequential
- Parallel using OpenMP
- Parallel using threads (bad performance due to lack of pooling)
- Parallel and distributed using OpenMPI (requires OpenMPI, see: https://www.open-mpi.org/)
- Parallel and distributed using gRPC (in NodeJS, see https://grpc.io/docs/languages/node/quickstart/)

## OpenMPI

OpenMPI version must currently be built and executed as a separated file.
Use `mpicxx openmpi.cpp utils.cpp -o openmpi.exe -I .` for compiling and `mpirun openmpi.exe` to run it.

## gRPC with NodeJS

To run go to nodejs-grpc directory, download packages by `npm install` and start servers by `node server.js`.
The gRPC command can be executed from another terminal with `node client.js`.
