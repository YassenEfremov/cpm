#include "version.h"

#include <iostream>
#include <cstdlib>


/* ============================================================================================= */
/* Classes, Structures, Global variables, Function declarations */



/* ============================================================================================= */


int main(int argc, char *argv[]) {

    std::cout << "cpm v" << CPM_VERSION_MAJOR << "."
                          << CPM_VERSION_MINOR << "."
                          << CPM_VERSION_PATCH << "\n";

    return EXIT_SUCCESS;
}


/* ============================================================================================= */
/* Function definitions */



/* ============================================================================================= */
