#name: Bad MVE VADC instructions
#as: -march=armv8.1-m.main+mve.fp
#error_output: mve-vadc-bad.l

.*: +file format .*arm.*
