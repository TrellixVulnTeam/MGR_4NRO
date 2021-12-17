 function [a_ret] = alfa_max(f,a0)

a1 = a0;
f1 = f(a1);
delta = 0.05;

a = a1 + delta;
fa = f(a);

if  f1 <= fa
    a3 = a;
    a2 = (a1 + a3) / 2;
    f2 = f(a2);
    f3 = f(a3);

    while ~(f1 > f2 && f2 < f3)
        a3 = a2;
        f3 = f2;
        a2 = (a1 + a2)/2;
        f2 = f(a2);
    end

    a_ret = a3;
else
    a2 = a;
    a3 = a2 + delta;
    f2 = f(a2);
    f3 = f(a3);

    while ~(f1 > f2 && f2 < f3)
        delta = 2 * delta;
        a3 = a2 + delta;
        f3 = f(a3);
    end
    
    a_ret = a3;

end

    