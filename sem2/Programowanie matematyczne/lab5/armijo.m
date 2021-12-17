function [alfa_Armijo,it] = armijo(f,df, kappa, ro, eps)

it = 0;
alfa_k = 1;
f0 = f(0);
df0 = df(0);
f_ro = @(a)(f0+ro*a*df0);

while f(alfa_k) > f_ro(alfa_k)
    alfa_k = alfa_k / kappa;
    it = it + 1;
    if(alfa_k < eps)
        alfa_Armijo = alfa_k;
        return;
    end
end

alfa_Armijo = alfa_k;
end
