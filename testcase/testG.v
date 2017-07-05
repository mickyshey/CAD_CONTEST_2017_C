module top ( y1 , y2 , a , b , c );
input a , b , c ;
output y1 , y2 ;
wire g1 , g2 , g3 , g4 , g5 ;

and ( g1 , a , b );
and ( g2 , b , c );
or ( g3 , g1 , g2 );
and ( g4 , a , c );
or ( y1 , g3 , g4 );
buf ( g5 , 1'b0 );
buf ( y2 , g5 );

endmodule
