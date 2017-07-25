V3_PATH=../../v3 # v3 directory, need to change

verify.py ../testdata/cad16_np3_case/case$1/cir1.v ../testdata/cad16_np3_case/case$1/cir2.v ../result/match_$1.out
$V3_PATH/v3 -f verify.dofile
