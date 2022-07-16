const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');

const PROTO_PATH = './server.proto';

// gRPC Proto definition
const packageDefinition = protoLoader.loadSync(PROTO_PATH);
const newsProto = grpc.loadPackageDefinition(packageDefinition);

// Servers definition
const root = new grpc.Server();
const workers = [...Array(4).keys()].map(() => new grpc.Server());

// Connection to workers definition
const WorkerService = grpc.loadPackageDefinition(
    packageDefinition,
).WorkerService;
const workerClients = workers.map((_, index) => {
  return new WorkerService(
      `localhost:5005${index + 1}`,
      grpc.credentials.createInsecure(),
  );
});

root.addService(newsProto.RootService.service, {
  rpcGaussianElimination: async (call, callback) => {
    const {rows} = call.request;
    const chunkSize = rows / workers.length;
    // Because of augmented part
    const cols = rows * 2;

    let error = null;
    let {data} = call.request;

    console.log('Root received data: ', call.request);

    try {
      for (let diagonal = 0; diagonal < rows; diagonal++) {
        let pivotRow = diagonal;
        let pivotValue = data[diagonal + pivotRow * cols];

        // Find better pivot if it exists
        for (let row = diagonal + 1; row < rows; row++) {
          const temp = data[diagonal + row * cols];
          if (Math.abs(temp > pivotValue)) {
            pivotRow = row;
            pivotValue = temp;
          }
        }

        // If there's a 0 on a diagonal matrix is singular and
        // doesn't have an inverse
        if (data[diagonal + pivotRow * cols] === 0) {
          throw new Error('Error: Provided matrix cannot be inverted');
        }

        // Swap the greatest value row with current row
        if (pivotRow !== diagonal) {
          for (let col = 0; col < cols; col++) {
            const temp = data[col + diagonal * cols];
            data[col + diagonal * cols] = data[col + pivotRow * cols];
            data[col + pivotRow * cols] = temp;
          }
        }

        // Make pivotValue 1 and compute other cols
        for (let col = diagonal; col < cols; col++) {
          data[col + diagonal * cols] /= pivotValue;
        }

        const resolutions = await Promise.all(workerClients.map(
            (worker, index) => new Promise(
                async (resolve, reject) => worker.rpcReduceDiagonal(
                    {
                      data: data.slice(
                          chunkSize * index * cols,
                          chunkSize * index * cols + chunkSize * cols,
                      ),
                      diagonalArray: data.slice(
                          diagonal * cols, diagonal * cols + cols,
                      ),
                      diagonal,
                      rows: chunkSize,
                    },
                    (err, {data: chunkData}) => {
                      if (err) reject(err);

                      // console.log(index);

                      data.splice(
                          chunkSize * index * cols,
                          chunkSize * index * cols + chunkSize * cols,
                          ...chunkData,
                      );

                      // console.log(data);

                      resolve(chunkData);
                    }),
            ),
        ));

        // Map reduce :)
        data = resolutions.reduce((prev, curr) => [...prev, ...curr]);
      }
    } catch (err) {
      console.error(err);
      error = err;
    } finally {
      callback(error, {data});
    }
  },
});

workers.forEach((worker, index) => {
  worker.addService(newsProto.WorkerService.service, {
    rpcReduceDiagonal: (call, callback) => {
      const {data, diagonalArray, diagonal, rows} = call.request;
      // Because of augmented part
      const cols = diagonalArray.length;

      let error = null;

      console.log(`Worker ${index} received data: `, call.request);

      try {
        for (let row = 0; row < rows; row++) {
          if (rows * index + row === diagonal) {
            continue;
          }

          const ratio = data[diagonal + row * cols];

          // Reduce row elements by pivot * factor
          for (let col = diagonal; col < cols; col++) {
            data[col + row * cols] -= diagonalArray[col] * ratio;
          }
        }
      } catch (err) {
        console.err(err);
        error = err;
      } finally {
        callback(error, {data});
      }
    },
  });
});

root.bindAsync(
    '127.0.0.1:50050',
    grpc.ServerCredentials.createInsecure(),
    () => {
      console.log('Root running at http://127.0.0.1:50050');
      root.start();
    },
);

workers.forEach((worker, index) => {
  const location = `127.0.0.1:5005${index + 1}`;

  worker.bindAsync(
      location,
      grpc.ServerCredentials.createInsecure(),
      () => {
        console.log(`Worker running at ${location}`);
        worker.start();
      },
  );
},

);
