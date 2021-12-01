rng(1);
clear;
clc;
opcje = optimset(@linprog);
opcje = optimset(opcje,'Display','none');
N = 10;

wrong_dual = 0;
wrong_dual_val = 0;
wrong_val = 0;
diff_vec = 0;
my_better = 0;
total1 = 0;
total2 = 0;
max1 = 0;
max2 = 0;
for i = 1:N
    Losowanie
    [x,fval,exitflag,output,lambda] = linprog(-c,A,b,[],[],-g,g,opcje);
    [c2,A2,b2]=get_dual(c,A,b,g);
    %[ROx,ROy,exitflagg]=sympleks(c,A,b,g)

    lb = zeros(15,1);
    ub = Inf*ones(15,1);
    [x2,fval2,exitflag2,output2,lambda2] = linprog(c2,[],[],-A2,-b2,lb,ub,opcje);
    j = output2.iterations;
    total1 = total1 + j;
    [x_my,retval,j2,exitflag_my, x_primary] = compute_dual(-c2,A2,b2);
    total2 = total2 + j2;
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
    if j2 < j
        my_better = my_better + 1;
    end
    if j > max1
        max1 = j;
    end
    if j2 > max2
        max2 = j2;
    end
    if norm(x_primary+x') > 1e-6
        diff_vec = diff_vec + 1;
    end
    %lambda
    %lambda2
end

wrong_dual
wrong_dual_val
wrong_val
my_better
diff_vec
mean1 = total1/N
mean2 = total2/N
max1
max2