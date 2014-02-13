# -*- coding: utf-8 -*-

import glob
import os
BASE_PATH = os.path.dirname(os.path.abspath(__file__)) + "/../"


def main():
    wsj_cps = glob.glob(BASE_PATH + "raw_data/wsj*")

    train_f = open(BASE_PATH + "data/train.txt", "w")
    for cps_path in wsj_cps[0:-2]:
        read_out(train_f, cps_path)
    train_f.close()

    test_f  = open(BASE_PATH + "data/test.txt", "w")
    for cps_path in wsj_cps[-1:]:
        read_out(test_f, cps_path)
    test_f.close()


def read_out(f_obj, cps_path):
    cps = open(cps_path, "r")

    is_start = True
    for raw_line in cps:
        line = raw_line.rstrip("\n\r")
        if line == "":
            f_obj.write("\n")
            is_start = True
            continue

        fields  = line.split("\t")
        if is_start:
            f_obj.write(fields[1])
            is_start = False
        else:
            f_obj.write(" " + fields[1])

    cps.close()


if __name__ == "__main__":
    main()
