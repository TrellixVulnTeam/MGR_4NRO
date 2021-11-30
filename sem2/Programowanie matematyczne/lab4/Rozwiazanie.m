rng(1);
clear;
opcje = optimset(@linprog);
opcje = optimset(opcje,'Display','none');
N = 1;
for i = 1:N
    Losowanie
    [x,fval,exitflag,output] = linprog(-c,A,b,[],[],-g,g,opcje);
    [c2,A2,b2]=get_dual(c,A,b,g);

    lb=zeros(15,1);
    ub=Inf*ones(15,1);
    [x2,fval2,exitflag2,output2] = linprog(c2,-A2,-b2,[],[],lb,ub,opcje);
    
    compute_dual(c2,A2,b2);

    if (fval2+fval)>1e-6
        fval
        fval2
    end
end