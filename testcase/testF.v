module top ( y1 , y2 , a , b , c );
input a , b , c ;
output y1 , y2 ;
wire g1 , g2 , g3 ;
wire t_0 ;

and ( g1 , b , c );
or ( g2 , a , g1 );
and ( y1 , g2 , t_0 );
buf ( g3 , 1'b0 );
buf ( y2 , g3 );

endmodule
