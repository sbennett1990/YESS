# sed(1) script to "fix" a dump instruction.
# To get dump, place the following instruction into your assembly:
#
# call $0x07 # dump
#
# This script will turn it into a dump instruction that the yess
# program understands.
#
# Invoke with: sed -E -i.orig -f fixdump.sed prog.yo

s/^(  0x[0-9a-f]{3}:) (8001000000)(   \|.*# dump.*)/\1 c001000000\3/g
s/^(  0x[0-9a-f]{3}:) (8005000000)(   \|.*# dump.*)/\1 c005000000\3/g
s/^(  0x[0-9a-f]{3}:) (8007000000)(   \|.*# dump.*)/\1 c007000000\3/g
