module top ( y , a , b , c );
input a , b , c ;
output y ;
wire g1 , g2 ;
wire t_0 ;

and ( g1 , b , c );
or ( g2 , a , g1 );
and ( y , g2 , t_0 );

endmodule
