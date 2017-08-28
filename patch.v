module patch (w4, g2, g1);
input g2, g1;
output w4;
wire w0, w1, w2, w3, w4;
or (w0, g1, g2);
or (w1, w0, g2);
or (w2, w1, g1);
or (w3, g2, w2);
or (w4, w3, g1);
endmodule