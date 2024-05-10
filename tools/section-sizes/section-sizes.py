import os
import re
import sys

def main():

    disassemblyDir = sys.argv[1]
    kyberVariant = sys.argv[2]

    KyberBaseObj = ["cbd", "indcpa", "kem", "ntt", "poly", "polyvec", "reduce", "rng", "verify", "ctr_prng"]
    KyberSymObj = {"KECCAK": ["symmetric-shake"], "90s": ["kyber_sha256", "kyber_sha512", "symmetric-aes"], "ASCON": ["symmetric-ascon"]}
    CommonSymObj = {"ASCON": ["ascon_decrypt", "ascon_encrypt", "ascon_hash", "permutations"], "KECCAK": ["sha3_api", "sha3_f1600_rvb32", "aes_encrypt_t_table", "ccm_mode", "utils"], "90s" : ["sha256", "sha512", "aes_encrypt_t_table", "ccm_mode", "utils"]}

    textSecSizeMatcher = re.compile("\.text\.([a-zA-Z0-9_-]*)\s+([a-zA-Z0-9]+)")
    dataSecSizeMatcher = re.compile("(\..*data[.a-zA-Z0-9-_]*)\s+([a-zA-Z0-9]+)\s+")
    headerFinishedMatcher = re.compile("Disassembly of section\s")

    # print(os.listdir(disassemblyDir))

    textSizes = {obj: {} for obj in os.listdir(disassemblyDir)}
    dataSizes = {obj: {} for obj in os.listdir(disassemblyDir)}
    textTotalSizes = {obj: 0 for obj in os.listdir(disassemblyDir)}
    dataTotalSizes = {obj: 0 for obj in os.listdir(disassemblyDir)}

    for disassemblyFileName in os.listdir(disassemblyDir):

        with open(disassemblyDir + "/" + disassemblyFileName) as disassemblyFile:

            for line in disassemblyFile.readlines():

                # match = re.match(textSecSizeMatcher, line)
                match = textSecSizeMatcher.search(line)
                if match:
                    textSizes[disassemblyFileName][match.group(1)] = int(match.group(2), 16)

                # match = re.match(dataSecSizeMatcher, line)
                match = dataSecSizeMatcher.search(line)
                if match:
                    dataSizes[disassemblyFileName][match.group(1)] = int(match.group(2), 16)

                # if re.match(headerFinishedMatcher, line):
                if headerFinishedMatcher.search(line):
                    break

    print()
    print("---- Text Section Sizes:")

    for obj, sections in textSizes.items():

        print(f"\t{obj}:")

        totalSize = 0

        for section, size in sections.items():

            print(f"\t\t{size}: {section}")
            totalSize += size

        print(f"\t\t{totalSize}: TOTAL .TEXT SIZE\n")
        textTotalSizes[obj] = totalSize

    # print("\tKyber total .text size:")
    print("\tCryptosystem total .text size:")
    totalSize = 0

    # for obj in KyberBaseObj:
    # for obj in KyberBaseObj + KyberSymObj[kyberVariant]:
    for obj in KyberBaseObj + KyberSymObj[kyberVariant] + CommonSymObj[kyberVariant]:
        totalSize += textTotalSizes[obj + ".dis"]
        print(f"\t\t{textTotalSizes[obj + '.dis']}: {obj}")

    # for obj in KyberSymObj[kyberVariant]:
    #     totalSize += textTotalSizes[obj + ".dis"]
    #     print(f"\t\t{textTotalSizes[obj + ".dis"]}: {obj}")

    print(f"\t\t{totalSize}: TOTAL .TEXT SIZE\n")

    print("---- Data Section Sizes:")

    for obj, sections in dataSizes.items():

        print(f"\t{obj}:")

        totalSize = 0

        for section, size in sections.items():

            print(f"\t\t{size}: {section}")
            totalSize += size

        print(f"\t\t{totalSize}: TOTAL .*DATA SIZE\n")
        dataTotalSizes[obj] = totalSize

    # print("\tKyber total .*data size:")
    print("\tCryptosystem total .*data size:")
    totalSize = 0

    # for obj in KyberBaseObj + KyberSymObj[kyberVariant]:
    for obj in KyberBaseObj + KyberSymObj[kyberVariant] + CommonSymObj[kyberVariant]:
        totalSize += dataTotalSizes[obj + ".dis"]
        print(f"\t\t{dataTotalSizes[obj + '.dis']}: {obj}")

    # for obj in KyberSymObj[kyberVariant]:
    #     totalSize += dataTotalSizes[obj + ".dis"]
    #     print(f"\t\t{dataTotalSizes[obj + ".dis"]}: {obj}")

    print(f"\t\t{totalSize}: TOTAL .*DATA SIZE\n")

if __name__ == "__main__":
    main()
