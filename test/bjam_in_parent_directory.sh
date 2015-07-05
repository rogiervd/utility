#!/bin/bash

# Run this from the root directory of the "utility" repository.

# This script will generate a directory ../utility-test and not remove it!
# So watch out where you run it.

# Run the tests by using this repository as submodule of the "utility-test"
# repository.

# This is necessary because this repository cannot be tested by itself.


# Travis CI looks for this line.
set -ev

set -o nounset
set -o errexit

(
    # Check out the "master" branch of "utility-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/utility-test.git
    cd utility-test
    git checkout master
    git submodule init
    git submodule update
    # Then replace the "utility" submodule with the one in ../utility.
    rm -r utility
    ln -s ../utility

    # Run the tests.
    bjam "$@"

)
