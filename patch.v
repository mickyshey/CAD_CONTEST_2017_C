module patch (w360, g1, g5, n29);
input g1, g5, n29;
output w360;
wire w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15, w16, w17, w18, w19, w20, w21, w22, w23, w24, w25, w26, w27, w28, w29, w30, w31, w32, w33, w34, w35, w36, w37, w38, w39, w40, w41, w42, w43, w44, w45, w46, w47, w48, w49, w50, w51, w52, w53, w54, w55, w56, w57, w58, w59, w60, w61, w62, w63, w64, w65, w66, w67, w68, w69, w70, w71, w72, w73, w74, w75, w76, w77, w78, w79, w80, w81, w82, w83, w84, w85, w86, w87, w88, w89, w90, w91, w92, w93, w94, w95, w96, w97, w98, w99, w100, w101, w102, w103, w104, w105, w106, w107, w108, w109, w110, w111, w112, w113, w114, w115, w116, w117, w118, w119, w120, w121, w122, w123, w124, w125, w126, w127, w128, w129, w130, w131, w132, w133, w134, w135, w136, w137, w138, w139, w140, w141, w142, w143, w144, w145, w146, w147, w148, w149, w150, w151, w152, w153, w154, w155, w156, w157, w158, w159, w160, w161, w162, w163, w164, w165, w166, w167, w168, w169, w170, w171, w172, w173, w174, w175, w176, w177, w178, w179, w180, w181, w182, w183, w184, w185, w186, w187, w188, w189, w190, w191, w192, w193, w194, w195, w196, w197, w198, w199, w200, w201, w202, w203, w204, w205, w206, w207, w208, w209, w210, w211, w212, w213, w214, w215, w216, w217, w218, w219, w220, w221, w222, w223, w224, w225, w226, w227, w228, w229, w230, w231, w232, w233, w234, w235, w236, w237, w238, w239, w240, w241, w242, w243, w244, w245, w246, w247, w248, w249, w250, w251, w252, w253, w254, w255, w256, w257, w258, w259, w260, w261, w262, w263, w264, w265, w266, w267, w268, w269, w270, w271, w272, w273, w274, w275, w276, w277, w278, w279, w280, w281, w282, w283, w284, w285, w286, w287, w288, w289, w290, w291, w292, w293, w294, w295, w296, w297, w298, w299, w300, w301, w302, w303, w304, w305, w306, w307, w308, w309, w310, w311, w312, w313, w314, w315, w316, w317, w318, w319, w320, w321, w322, w323, w324, w325, w326, w327, w328, w329, w330, w331, w332, w333, w334, w335, w336, w337, w338, w339, w340, w341, w342, w343, w344, w345, w346, w347, w348, w349, w350, w351, w352, w353, w354, w355, w356, w357, w358, w359, w360;
not(w0, n29);
not(w1, g1);
not(w2, g5);
or (w3, g1, w2);
not(w4, g5);
not(w5, g1);
or (w6, w5, g5);
not(w7, n29);
not(w8, g1);
not(w9, g5);
or (w10, g1, g5);
not(w11, g5);
or (w12, g1, w11);
not(w13, g1);
or (w14, w13, g5);
not(w15, g1);
not(w16, g5);
or (w17, w15, w16);
not(w18, g1);
not(w19, g5);
or (w20, g1, w10);
or (w21, w20, g1);
or (w22, w21, n29);
or (w23, w22, n29);
or (w24, w23, n29);
or (w25, w19, w18);
or (w26, w9, w8);
or (w27, w26, w25);
or (w28, w27, w17);
and (w29, w28, w21);
and (w30, w3, n29);
or (w31, g5, w3);
and (w32, n29, w7);
or (w33, w32, w4);
or (w34, w33, w1);
and (w35, w34, w31);
or (w36, w35, w29);
or (w37, w36, w30);
and (w38, w37, w24);
or (w39, w21, n29);
or (w40, w39, n29);
or (w41, w40, n29);
and (w42, w3, w24);
and (w43, w42, w41);
or (w44, w43, g5);
or (w45, w0, w12);
or (w46, w45, w3);
and (w47, w46, w44);
or (w48, w47, g1);
or (w49, w48, g1);
or (w50, w8, w18);
or (w51, w8, w18);
or (w52, w51, w50);
and (w53, w24, w0);
or (w54, w53, w8);
or (w55, w54, w18);
or (w56, w55, w52);
or (w57, w56, w14);
or (w58, w57, w6);
or (w59, w58, g5);
or (w60, w0, w18);
and (w61, w60, n29);
and (w62, w0, n29);
or (w63, w62, w18);
or (w64, w63, w25);
or (w65, w64, w61);
or (w66, w65, w17);
and (w67, n29, w7);
or (w68, w67, w9);
or (w69, w68, w8);
or (w70, w69, w66);
or (w71, w70, w61);
or (w72, w71, w17);
or (w73, w72, w4);
and (w74, w73, w59);
or (w75, w74, w1);
and (w76, w75, w49);
and (w77, w76, w38);
or (w78, w10, g5);
and (w79, w78, w3);
or (w80, w79, w0);
and (w81, w80, n29);
or (w82, w7, g5);
or (w83, w82, g5);
and (w84, w83, w12);
or (w85, w84, w0);
and (w86, w85, w41);
or (w87, w86, w81);
or (w88, w87, g1);
or (w89, w88, g1);
or (w90, w89, g1);
or (w91, w0, w8);
or (w92, w91, w18);
or (w93, w92, w52);
or (w94, w93, w14);
and (w95, w94, w59);
or (w96, w95, w6);
or (w97, w96, g5);
and (w98, n29, w7);
or (w99, w98, w9);
or (w100, w99, w8);
or (w101, w100, w61);
or (w102, w101, w17);
or (w103, w102, w66);
or (w104, w103, w4);
and (w105, w104, w97);
or (w106, w105, w1);
and (w107, w106, w90);
and (w108, w107, w77);
or (w109, w108, w77);
or (w110, w109, w108);
or (w111, w77, w110);
or (w112, w111, w108);
or (w113, w112, w108);
or (w114, w19, w9);
and (w115, w114, g5);
and (w116, w17, w10);
or (w117, w8, w66);
and (w118, w117, w41);
or (w119, w118, w115);
or (w120, w119, w116);
or (w121, w120, n29);
or (w122, w121, n29);
or (w123, w122, n29);
or (w124, w123, w115);
or (w125, w124, w116);
or (w126, w125, n29);
or (w127, w126, n29);
or (w128, w127, n29);
or (w129, w7, w81);
and (w130, w129, w41);
or (w131, w130, g1);
or (w132, w131, g5);
and (w133, w132, w128);
and (w134, w133, w128);
or (w135, w0, w115);
or (w136, w135, w81);
or (w137, w136, w12);
and (w138, w137, w31);
or (w139, w138, w115);
or (w140, w61, w7);
and (w141, w140, w123);
or (w142, w141, w17);
or (w143, w142, w1);
and (w144, w143, w139);
or (w145, w144, w4);
or (w146, w115, w0);
and (w147, w146, w24);
or (w148, w147, w6);
or (w149, w148, w14);
and (w150, w149, w132);
and (w151, w150, w145);
and (w152, w151, w134);
or (w153, w152, w115);
and (w154, w153, w128);
or (w155, w154, w115);
or (w156, w113, n29);
or (w157, w156, n29);
or (w158, w112, w0);
and (w159, w158, w157);
or (w160, w116, g5);
or (w161, w160, n29);
and (w162, w161, w0);
or (w163, w17, n29);
and (w164, w163, w162);
and (w165, w164, w0);
or (w166, g5, w14);
or (w167, w166, w6);
or (w168, w12, w3);
and (w169, w168, w167);
and (w170, w169, n29);
or (w171, w167, w0);
or (w172, w12, w3);
and (w173, w172, w171);
and (w174, w173, n29);
or (w175, w174, w165);
or (w176, w175, w170);
or (w177, w176, w159);
and (w178, w177, w128);
or (w179, w178, w155);
and (w180, w179, w128);
or (w181, w116, w115);
or (w182, w181, n29);
or (w183, w182, n29);
or (w184, w183, n29);
and (w185, w184, w180);
or (w186, w10, w115);
or (w187, w186, n29);
or (w188, w187, n29);
or (w189, w188, n29);
or (w190, w115, w14);
or (w191, w190, w6);
or (w192, w191, w0);
and (w193, w192, w189);
and (w194, w193, w185);
or (w195, w194, g5);
and (w196, w195, w180);
or (w197, w41, n29);
or (w198, w197, w196);
and (w199, w198, w180);
or (w200, w41, n29);
or (w201, w8, w18);
or (w202, w201, w0);
and (w203, w202, w200);
and (w204, w203, w199);
or (w205, w204, w196);
and (w206, w205, w180);
or (w207, n29, w185);
or (w208, w116, w115);
or (w209, w208, n29);
or (w210, w209, n29);
or (w211, w210, n29);
or (w212, w115, w12);
or (w213, w212, w3);
or (w214, w213, w0);
and (w215, w214, w211);
and (w216, w215, w207);
and (w217, w216, w180);
or (w218, w123, n29);
or (w219, w218, w217);
and (w220, w219, w206);
and (w221, w220, w180);
or (w222, w123, n29);
or (w223, g1, w0);
and (w224, w223, w222);
and (w225, w224, w221);
or (w226, w225, w217);
and (w227, w226, w206);
and (w228, w227, w180);
or (w229, w116, n29);
or (w230, w229, n29);
or (w231, w230, n29);
or (w232, w231, w115);
or (w233, w232, w228);
and (w234, w233, w180);
or (w235, w116, n29);
or (w236, w235, n29);
or (w237, w236, n29);
or (w238, w237, w115);
or (w239, w12, w3);
and (w240, w239, w171);
or (w241, w240, w0);
and (w242, w241, w238);
and (w243, w242, w234);
or (w244, w243, w115);
or (w245, w244, w228);
and (w246, w245, w180);
and (w247, w128, w246);
and (w248, w247, w246);
and (w249, w248, w246);
and (w250, w249, w246);
and (w251, w246, w250);
and (w252, w251, w246);
and (w253, w252, w246);
and (w254, w253, w246);
and (w255, w254, w246);
and (w256, w255, w250);
and (w257, w256, w246);
and (w258, w257, w246);
and (w259, w258, w246);
and (w260, w246, w259);
and (w261, w246, w260);
and (w262, w261, w259);
and (w263, w246, w262);
and (w264, w263, w260);
and (w265, w264, w259);
or (w266, w115, w123);
or (w267, w266, w116);
or (w268, w267, n29);
or (w269, w268, n29);
or (w270, w269, n29);
and (w271, w12, w14);
and (w272, w6, w81);
or (w273, w272, w115);
or (w274, w273, w115);
or (w275, w8, w18);
and (w276, g1, w275);
and (w277, w3, w167);
or (w278, w277, w276);
or (w279, w278, w274);
or (w280, w279, w271);
or (w281, w280, w0);
and (w282, w281, w246);
and (w283, w282, w246);
and (w284, w283, w270);
or (w285, w115, w123);
or (w286, w285, w116);
or (w287, w286, n29);
or (w288, w287, n29);
or (w289, w288, n29);
and (w290, w289, w246);
and (w291, w290, w284);
and (w292, w291, w270);
or (w293, w274, w271);
or (w294, w293, w0);
and (w295, w294, w211);
and (w296, w295, w246);
and (w297, w296, w246);
and (w298, w297, w284);
and (w299, w298, w292);
and (w300, w299, w265);
and (w301, w300, w259);
or (w302, w222, w238);
or (w303, w302, w301);
and (w304, w303, w246);
and (w305, w304, w246);
and (w306, w305, w284);
and (w307, w306, w292);
and (w308, w307, w265);
and (w309, w308, w259);
or (w310, w3, w271);
and (w311, w310, w167);
or (w312, w311, w0);
and (w313, w312, w309);
or (w314, w313, w276);
or (w315, w314, w301);
and (w316, w315, w265);
and (w317, w316, w259);
and (w318, w292, w317);
and (w319, w284, w318);
and (w320, w319, w317);
or (w321, w115, n29);
or (w322, w321, n29);
or (w323, w322, n29);
and (w324, w323, w317);
or (w325, w324, w116);
and (w326, w0, w246);
and (w327, w326, w320);
and (w328, w327, w317);
or (w329, w328, w274);
or (w330, w329, w271);
and (w331, w330, w325);
or (w332, w276, w331);
or (w333, w332, w331);
or (w334, w331, w333);
or (w335, w334, w331);
or (w336, w335, w331);
or (w337, w336, n29);
or (w338, w337, n29);
or (w339, w338, n29);
and (w340, w339, w317);
or (w341, w340, w116);
and (w342, w271, w341);
and (w343, w341, w317);
and (w344, w343, w246);
and (w345, w344, w320);
and (w346, w0, w345);
or (w347, w346, w342);
or (w348, w346, w331);
or (w349, w348, w331);
or (w350, w349, w333);
or (w351, w167, w350);
and (w352, w351, w341);
or (w353, w352, w346);
or (w354, w347, w331);
and (w355, w353, w341);
and (w356, w3, w353);
and (w357, w356, w355);
or (w358, w354, w350);
or (w359, w358, w331);
or (w360, w359, w357);
endmodule