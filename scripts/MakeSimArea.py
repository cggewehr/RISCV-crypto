#!/usr/bin/env python3

def MakeSimArea(DirName):

    import os
    import shutil

    RootPath = os.path.abspath(__file__ + "/..")
    SimPath = os.path.abspath(RootPath + "/build/" + DirName)

    if os.path.exists(SimPath):

        while True:

            ipt = input()

            print("Warning: Dir <" + SimPath + "> already exists. Do you wish to proceed (Y/N)?")
            if ipt == "Y" or ipt == "y":
                break  # TODO: Prompt if sim dir should be wiped clean
            elif ipt == "N" or ipt == "n":
                exit(0)

    # Make dir in RISCV-crypto/build/sim/($DIR_NAME)
    os.makedirs(SimPath, exist_ok = True)  # exist_ok argument to makedirs() only works for Python 3.2+

    # TODO: Create makefile (includes common makefiles)
    with open(SimPath + "/makefile") as makefile:
        makefile.write(f"ROOT_PATH=  {RootPath}\n")
        makefile.write(f"SIM_PATH=  {SimPath}\n")
        makefile.write(f"VENDOR = Cadence\n")
        makefile.write(f"include $\{{RootPath}\}/scripts/sim_common.mk\n")
        makefile.write(f"include $$\{{RootPath}\}/sw/common/common.mk\n")

    # Create default parameter file in sim area
    shutil.copyfile(RootPath + "/scripts/template/param.txt", SimPath + "param.txt")

if __name__ == "__main__":

    import argparse

    # TODO: Get args from command line

    MakeSimArea(**vars(args))
