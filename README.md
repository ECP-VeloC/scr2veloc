# scr2veloc
Library to translate SCR calls to VELOC calls.
This installs the scr.h header and libscr files.

# Build scr2veloc library
Requires path to veloc install.

    #!/bin/bash

    # variables to set if need to build with different compiler
    export CC=icc
    export CXX=icpc
    export F77=ifort
    export F90=ifort

    VELOC_PATH=../veloc.git/install

    cmake -DCMAKE_INSTALL_PREFIX=`pwd`/install -DCMAKE_BUILD_TYPE=Debug -DWITH_VELOC_PREFIX=$VELOC_PATH -DWITH_KVTREE_PREFIX=$VELOC_PATH

    make VERBOSE=1

    make install

# Build SCR example programs linked to scr2veloc
Copy the examples from the install location:

    #!/bin/bash
    cp -pr <scr2veloc>/install/share/scr/examples .
    cd examples
    make

# Run SCR examples
Create a veloc config file, e.g.,:

    >>: cat veloc.config
    scratch = /tmp/scratch
    persistent = /tmp/persistent
    max_versions = 1
    mode = sync
    axl_type = AXL_XFER_SYNC

To run, set SCR2VELOC\_CONFIG to point to the veloc config file,
and set SCR2VELOC\_NAMES to point to where the kvtree file should
be written/read that records the mapping of SCR output name to VELOC checkpoint id:

    #!/bin/bash
    export SCR2VELOC_CONFIG="veloc.config"
    export SCR2VELOC_NAMES="scrnames.kvtree"
    srun -n2 -N1 ./test_api
