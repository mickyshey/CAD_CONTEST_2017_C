if [ "$#" -ne "1" ]; then
   echo "Usage: $0 <testcase index>";
   exit 0;
fi

./mlllk testcase/unit$1/F.v testcase/unit$1/G.v testcase/unit$1/weight.txt patch.v out.v
