function [x, retval, j, exitflag] = my_linprog(c, A, b, ub)

    c = [c -c zeros(1, 15)];
    A = [A -A diag(ones(1,5)) zeros(5,10)];
    A = [A; diag(ones(1,10)) zeros(10,5) diag(ones(1,10))];
    b = transpose([b ub ub]);
    base = (11:25);
  
    [x,retval,j,exitflag] = my_simplex(A,b,c,base,0);
end