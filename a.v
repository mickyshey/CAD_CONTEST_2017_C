module top ( y1 , a , b );
input a , b ;
output y1 ;
wire g1 , g2 ;

and ( g1 , a , b );
and ( g2 , b , a );
xor ( y1 , g1 , g2 );

endmodule
