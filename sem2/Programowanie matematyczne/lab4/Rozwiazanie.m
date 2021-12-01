rng(1);
clear;
clc;
opcje = optimset(@linprog);
opcje = optimset(opcje,'Display','none');
N = 10;
for i = 1:N
    Losowanie
    [x,fval,exitflag,output] = linprog(-c,A,b,[],[],-g,g,opcje);

    [c2,A2,b2]=get_dual(c,A,b,g);

    lb=-Inf*ones(15,1);
    ub=zeros(15,1);
    [x2,fval2,exitflag2,output2] = linprog(c2,-A2,-b2,[],[],lb,ub,opcje);
    
    [x_my,retval,j,exitflag_my] = compute_dual(-c2,A2,b2);
    
    my_val = c * x_my;
    if abs(fval2+fval)>1e-6
        fval
        fval2
    end
    if abs(retval-fval)>1e-6
        fval
        retval
    end
end