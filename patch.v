module patch (w4, n2077, n1528, n1530);
input n2077, n1528, n1530;
output w4;
wire w0, w1, w2, w3, w4;
not(w0, n1530);
not(w1, n1528);
and (w2, w1, w0);
and (w3, n2077, w0);
or (w4, w2, w3);
endmodule