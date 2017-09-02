#!/usr/bin/python

import sys

def mySplit(s, sep):
  ret = []
  tmp = ""
  for c in s:
    if c in sep:
      if len(tmp) > 0:
        ret.append(tmp)
      tmp = ""
    else:
      tmp += c
  if len(tmp) > 0:
    ret.append(tmp)
  return ret

def parseVerilog(filename):
  f = open(filename, "r")
  name = ""
  inputPin = []
  outputPin = []
  for line in f:
    line = line.strip()
    if line[0:6] == "module":
      name = mySplit(line[6:], " (")[0]
    elif line[0:5] == "input":
      inputPin += mySplit(line[5:], " ,;")
    elif line[0:6] == "output":
      outputPin += mySplit(line[6:], " ,;")
  f.close()
  return name, inputPin, outputPin

def genMiter(cir1File, cir2File, matchFile):
  cir1Name, cir1Input, cir1Output = parseVerilog(cir1File)
  cir2Name, cir2Input, cir2Output = parseVerilog(cir2File)
  matchOut, matchIn = getMatch(matchFile)
  # substitute cir2 input
  miterInput = list(cir1Input)
  mark = 1
  for i in range(len(cir2Input)):
    if cir2Input[i] in matchIn:
      cir2Input[i] = getPhasePin(matchIn[cir2Input[i]])
    else:
      pinName = "tmp_input_" + str(mark)
      cir2Input[i] = pinName
      miterInput.append(pinName)
      mark += 1
  # tmp wires
  cir1Dict = {}
  cir2Dict = {}
  wire = []
  xorList = []
  orStr = "or( miter_output"
  mark = 1
  for i in range(len(cir1Output)):
    pinName = "tmp_output_" + str(mark)
    cir1Dict[cir1Output[i]] = pinName
    wire.append(pinName)
    mark += 1
  for i in range(len(cir2Output)):
    pinName = "tmp_output_" + str(mark)
    cir2Dict[cir2Output[i]] = pinName
    wire.append(pinName)
    mark += 1
  for out2 in matchOut:
    (phase, out1) = matchOut[out2]
    pinName = "tmp_output_" + str(mark)
    wire.append(pinName)
    xorList.append("xor( " + pinName + " , " + cir2Dict[out2] + " , " + ("" if phase == "+" else "!") + cir1Dict[out1] + " );")
    orStr += (" , " + pinName)
    mark += 1
  orStr += " );"

  # write miter circuit
  f = open("miter.v", "w")
  f.write("module miter( " + " , ".join(miterInput) + " , miter_output );\n")
  f.write("input " + " , ".join(miterInput) + ";\n")
  f.write("output miter_output;\n")
  f.write("wire " + " , ".join(wire) + ";\n")
  f.write("module_1_" + cir1Name + " inst1( " + " , ".join(cir1Input + [cir1Dict[p] for p in cir1Output]) + " );\n")
  f.write("module_2_" + cir2Name + " inst2( " + " , ".join(cir2Input + [cir2Dict[p] for p in cir2Output]) + " );\n")
  for l in xorList:
    f.write(l + "\n")
  f.write(orStr + "\n")
  f.write("endmodule\n")

  f.write("\n\n")

  g = open(cir1File)
  for line in g:
    if line[0:6] == "module":
      line = "module module_1_" + line[6:].lstrip()
    f.write(line)
  g.close()

  f.write("\n\n")

  g = open(cir2File)
  for line in g:
    if line[0:6] == "module":
      line = "module module_2_" + line[6:].lstrip()
    f.write(line)
  g.close()

  f.close()

def parse_verilog(circuit_file):
  f = open(circuit_file, "r")
  input_pin = []
  output_pin = []
  wire_pin = []
  for line in f:
    line = line.strip()
    if line[0:5] == "input":
      input_pin += mySplit(line[5:], " ,;")
    elif line[0:6] == "output":
      output_pin += mySplit(line[6:], " ,;")
    elif line[0:4] == "wire":
      wire_pin += mySplit(line[4:], " ,;")
  f.close()
  return wire_pin, input_pin, output_pin


def miter_f(patchFile, outFile):
  # collect wire in patch.v
  wires, base_node, output_pin = parse_verilog(patchFile)
  # write miter to a new file
  f = open("F_miter.v", "w")
  g = open(outFile, "r")
  for line in g:
    if line[0:8] == "wire t_0":
        #line = line[:8] + " , "
        line += "wire "
        line += " , ".join(wires)
        line += ";\n"
    if line[0:5] == "patch":
      i = 0;
      for x in output_pin:
        f.write("buf ( t_" + str(i) + ", " + x + " );\n")
        i += 1
      #f.write("buf ( t_0, " + output_pin[0] + " );\n") # FIXME: for multiple errors
      g.close()
      break
    f.write(line)
  g = open(patchFile, "r")
  for line in g:
    if line[0:6] == "module" or line[0:5] == "input" or line[0:6] == "output" or line[0:4] == "wire":
      continue
    f.write(line)
  f.close()


def main():
  if len(sys.argv) != 3:
    print( "Usage: " + sys.argv[0] + " <out.v> <patch.v>")
    return
  miter_f(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
  main()
