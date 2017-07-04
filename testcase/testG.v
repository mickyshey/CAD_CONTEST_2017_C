module top ( y , a , b , c );
input a , b , c ;
output y ;
wire g1 , g2 , g3 , g4 ;

and ( g1 , a , b );
and ( g2 , b , c );
or ( g3 , g1 , g2 );
and ( g4 , a , c );
or ( y , g3 , g4 );

endmodule
