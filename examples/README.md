## Building the Examples (using CMake)

If the examples require building, you can typically use CMake:

1.  Navigate to the `examples` directory:
    ```bash
    cd examples
    ```
2.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
3.  Configure the build using CMake:
    ```bash
    cmake ..
    ```
    (You might need to specify CMake options depending on your project setup, e.g., `-DCMAKE_BUILD_TYPE=Release`)
4.  Build the examples:
    ```bash
    make -j$(nproc)
    ```
5.  The compiled executables will typically be located in the `build` directory (or a subdirectory within it).

## Execute

1. Navigate to the directory where the relevant JSON configuration file is located
2. Run the examples:
    ```bash
    ../build/examples/bixit_examples
    ```
3. The result should be something similar to:
   ```bash
   [2025-05-11 15:59:11.742060] [info] Loading catalog from directory <./> ...
   [2025-05-11 15:59:11.747946] [info] Parsing 1 collected schema(s)
   [2025-05-11 15:59:11.748003] [debug] Parsing schema <rbc_pre_announcement> from <./rbc_pre_announcement.json> 
   [2025-05-11 15:59:11.764801] [debug] Inserting schema with key <rbc_pre_announcement>
   [2025-05-11 15:59:11.764867] [debug] Schema correctly parsed
   [2025-05-11 15:59:11.764991] [debug] Parsed 1 schema(s)
   {
    "NID_NRBCMESSAGE": 201,
    "M_MODE": 1,
    "NID_PACKET": 11,
    "L_PACKET": 1,
    "NC_CDTRAIN": 1,
    "NC_TRAIN": 1,
    "L_TRAIN": 1,
    "V_MAXTRAIN": 1,
    "M_LOADINGGAUGE": 1,
    "M_AXLELOADCAT": 1,
    "M_AIRTIGHT": 1,
    "N_AXLE": 1,
    "N_ITER_1": 2,
    "M_VOLTAGE": [
      1,
      0
    ],
    "NID_CTRACTION": [
      1
    ],
    "N_ITER_2": 2,
    "NID_NTC": [
      1,
      2
    ],
    "additional_packets": [
      {
        "NID_PACKET": 2,
        "Q_DIR": 3,
        "L_PACKET": 1,
        "M_VERSION": 7
      },
      {
        "NID_PACKET": 5,
        "L_PACKET": 7,
        "NID_OPERATIONAL": 21
      }
    ]
   }
   ```
