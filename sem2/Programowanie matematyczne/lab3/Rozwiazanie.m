rng(1);
clear;
opcje = optimset(@linprog);
opcje = optimset(opcje,'Display','none');
N = 1000;
wrong1 = 0;
wrong2 = 0;
different = 0;
total1 = 0;
total2 = 0;
my_better = 0;
max1 = 0;
max2 = 0;
for i = 1:N
    Losowanie
    [x,fval,exitflag,output] = linprog(-c,A,b,[],[],-g,g,[],opcje);
    j = output.iterations;
    total1 = total1 + j;
    [x2,retval,j2,exitflag2] = my_linprog(c,A,b,g);
    total2 = total2 + j2;
    if ~exitflag
        wrong1 = wrong1 + 1;
    end
    if ~exitflag2
        wrong2 = wrong2 + 1;
    end
    if (fval+retval) > 1e-10
        different = different + 1;
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
end

wrong1
wrong2
different
mean1 = total1/N
mean2 = total2/N
my_better
max1
max2