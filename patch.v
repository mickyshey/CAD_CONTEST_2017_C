module patch (w92, n29, n31, n33, n36);
input n29, n31, n33, n36;
output w92;
wire w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15, w16, w17, w18, w19, w20, w21, w22, w23, w24, w25, w26, w27, w28, w29, w30, w31, w32, w33, w34, w35, w36, w37, w38, w39, w40, w41, w42, w43, w44, w45, w46, w47, w48, w49, w50, w51, w52, w53, w54, w55, w56, w57, w58, w59, w60, w61, w62, w63, w64, w65, w66, w67, w68, w69, w70, w71, w72, w73, w74, w75, w76, w77, w78, w79, w80, w81, w82, w83, w84, w85, w86, w87, w88, w89, w90, w91, w92;
not(w0, n31);
not(w1, n33);
or (w2, n29, n36);
or (w3, w0, n36);
or (w4, w1, n36);
and (w5, w3, w4);
and (w6, w5, w2);
and (w7, w2, w3);
and (w8, w7, w6);
and (w9, w3, w4);
and (w10, w9, w2);
and (w11, w10, w8);
and (w12, w4, w3);
and (w13, w12, w2);
and (w14, w11, w2);
and (w15, w14, w2);
and (w16, w11, w2);
and (w17, w16, w2);
and (w18, w11, w17);
and (w19, w18, w15);
and (w20, w19, w2);
and (w21, w20, w10);
and (w22, w21, w17);
and (w23, w22, w15);
and (w24, w23, w2);
and (w25, w24, w10);
and (w26, w25, w17);
and (w27, w26, w15);
and (w28, w8, w27);
and (w29, w28, w13);
and (w30, w27, w13);
and (w31, w30, w27);
and (w32, w29, w27);
and (w33, w32, w27);
and (w34, w33, w27);
and (w35, w34, w27);
and (w36, w35, w27);
and (w37, w36, w2);
and (w38, w37, w2);
and (w39, w38, w2);
and (w40, w8, w31);
and (w41, w40, w27);
and (w42, w41, w27);
and (w43, w27, w7);
and (w44, w43, w31);
and (w45, w44, w27);
and (w46, w45, w27);
and (w47, w46, w27);
and (w48, w47, w31);
and (w49, w48, w27);
and (w50, w49, w27);
and (w51, w50, w27);
and (w52, w51, w27);
and (w53, w52, w42);
and (w54, w47, w31);
and (w55, w54, w27);
and (w56, w55, w27);
and (w57, w56, w27);
and (w58, w57, w27);
and (w59, w58, w53);
and (w60, w29, w27);
and (w61, w60, w27);
and (w62, w61, w27);
and (w63, w62, w59);
and (w64, w63, w53);
and (w65, w27, w39);
and (w66, w65, w39);
and (w67, w27, w39);
and (w68, w67, w39);
and (w69, w27, w68);
and (w70, w69, w66);
and (w71, w70, w64);
and (w72, w71, w39);
and (w73, w72, w27);
and (w74, w73, w39);
and (w75, w47, w31);
and (w76, w75, w27);
and (w77, w76, w27);
and (w78, w77, w27);
and (w79, w78, w27);
and (w80, w79, w27);
and (w81, w80, w27);
and (w82, w81, w2);
and (w83, w82, w39);
and (w84, w83, w2);
and (w85, w84, w39);
and (w86, w85, w74);
and (w87, w86, w27);
and (w88, w87, w27);
and (w89, w88, w27);
and (w90, w88, w27);
and (w91, w89, w86);
and (w92, w91, w90);
endmodule