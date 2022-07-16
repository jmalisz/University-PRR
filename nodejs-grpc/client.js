

const fs = require('fs');
const grpc = require('@grpc/grpc-js');
const protoLoader = require('@grpc/proto-loader');

const PROTO_PATH = './server.proto';

const packageDefinition = protoLoader.loadSync(PROTO_PATH);
const RootService = grpc.loadPackageDefinition(
    packageDefinition,
).RootService;
const client = new RootService(
    'localhost:50050',
    grpc.credentials.createInsecure(),
);

// Flattened
const matrix = [];

try {
  // Read from file an prepare for matrix
  const input = fs.readFileSync('../Input.txt', 'utf-8').trim().split(/[ ,\n]/);
  // Matrix size (since they are squared it's both rows and cols)
  const size = input.shift();

  // Augment with identity matrix
  for (let row = 0; row < size; row++) {
    for (let col = 0; col < size * 2; col++) {
      switch (true) {
        case col < size:
          matrix.push(input.shift());
          break;
        case row === col % size:
          matrix.push(1);
          break;
        case row !== col % size:
          matrix.push(0);
          break;
        default:
          throw new Error('Unexpected');
      }
    }
  }

  console.time('matrixInversion');

  client.rpcGaussianElimination(
      {rows: size, cols: size, data: matrix},
      (error, {data}) => {
        if (error) throw error;

        console.timeEnd('matrixInversion');

        const output = fs.createWriteStream(
            '../Output.txt',
            {flag: fs.O_TRUNC},
        );

        // Write to file stream with some adjustments for better reading
        data.forEach((elem, index) => {
          output.write(
            index === 0 || index % (size * 2) ? `${elem}\t` : `\n${elem}\t`,
          );
        });
      });
} catch (err) {
  console.error(err);
}

module.exports = client;

