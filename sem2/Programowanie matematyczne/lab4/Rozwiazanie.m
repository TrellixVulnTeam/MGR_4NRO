rng(1);
clear;
clc;
opcje = optimset(@linprog);
opcje = optimset(opcje,'Display','none');
N = 1000;

wrong_dual = 0;
wrong_dual_val = 0;
wrong_val = 0;

for i = 1:N
    Losowanie
    [x,fval,exitflag,output] = linprog(-c,A,b,[],[],-g,g,opcje);

    [c2,A2,b2]=get_dual(c,A,b,g);

    lb=-Inf*ones(15,1);
    ub=zeros(15,1);
    [x2,fval2,exitflag2,output2] = linprog(c2,-A2,-b2,[],[],lb,ub,opcje);
    
    [x_my,retval,j,exitflag_my, x_primary] = compute_dual(-c2,A2,b2);

    myval = c * x_primary';
    if abs(fval2+fval)>1e-6
        wrong_dual = wrong_dual + 1;
    end
    if abs(retval-fval)>1e-6
        wrong_dual_val = wrong_dual_val + 1;
    end
    if abs(myval-fval)>1e-6
        wrong_val = wrong_val + 1;
    end
end

wrong_dual
wrong_dual_val
wrong_val