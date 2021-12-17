function [alfa_gold,it] = gold(fun,alpha0,alphamax,eps)

c = (sqrt(5)-1)/2;
a = alpha0;
b = alphamax;

xk1 = b - c*(b-a);
xk2 = a + c*(b-a);
it = 0;
f1 = fun(xk1);
f2 = fun(xk2);
while abs(b-a)>eps

    it = it + 1;

    if f2 > f1
        b = xk2;
        xk2 = xk1;
        xk1 = b - c * (b-a);
        f2 = f1;
        f1 = fun(xk1);
    else
        a = xk1;
        xk1 = xk2;
        xk2 = a + c * (b-a);
        f1 = f2;
        f2 = fun(xk2);
    end
end
alfa_gold = (a+b)/2;
end