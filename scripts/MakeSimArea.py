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

    # Make sim dir in RISCV-crypto/build/sim/($DIR_NAME)
    os.makedirs(SimPath, exist_ok = True)  # exist_ok argument to makedirs() only works for Python 3.2+
    os.makedirs(SimPath + "/sw_build", exist_ok = True)
    os.makedirs(SimPath + "/log", exist_ok = True)

    # TODO: Create makefile (includes common makefiles)
    with open(SimPath + "/makefile") as makefile:
        makefile.write(f"ROOT_PATH={RootPath}\n")
        makefile.write(f"RISCV_CRYPTO_RTL={RootPath + '/src/rtl'}\n")
        makefile.write(f"RISCV_CRYPTO_TBENCH={RootPath + '/src/tbench'}\n")
        makefile.write(f"RISCV_CORE?=ibex-base\n\n")
        makefile.write(f"SIM_PATH={SimPath}\n")
        makefile.write(f"SW_BUILD_PATH={SimPath + '/sw_build'}\n")
        makefile.write(f"COMMON_DIR={RootPath + '/src/sw/common'}\n")
        makefile.write(f"VENDOR?=Cadence\n")
        makefile.write(f"PROG?=hello_test\n")
        makefile.write(f"include $\{{RootPath}\}/scripts/sim_common.mk\n")

    # Create default parameter file in sim area
    shutil.copyfile(RootPath + "/scripts/template/param.txt", SimPath + "param.txt")

if __name__ == "__main__":

    import argparse

    parser = argparse.ArgumentParser()
    
    parser.add_argument("--DirName", "-d", type = str, help = "Name of new sim area to be created within build/")

    args = parser.parse_args()

    MakeSimArea(**vars(args))
