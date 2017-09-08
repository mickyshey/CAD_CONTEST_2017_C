if [ "$#" -ne "1" ]; then
   echo "Usage: $0 <testcase index>";
   exit 0;
fi

./rpgen testcase/unit$1/F.v testcase/unit$1/G.v testcase/unit$1/weight.txt result/unit$1/patch.v result/unit$1/out.v
