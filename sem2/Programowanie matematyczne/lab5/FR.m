function [xFR,fval,it, GR_norms] = FR(alfafun,alfafun_d,df,x0,eps,alg, stop, showGRnorms)
%alg: 1-gold, 2-armijo, 3-fminbnd (else)
armijo_kappa = 1.2;
armijo_ro = 0.45;
armijo_eps = 1e-5;
GR_norms = [];

fval=0;
global x_;
global d_;
alfa0 = 0;
alfamax = alfa_max(alfafun,0);
it = 0;

gr_k = df(x0);
d_ = -gr_k;
x_ = x0;
if norm(gr_k) <= eps
    xFR = x_;
    return;
end

if alg == 1
    alfa = gold(alfafun,alfa0,alfamax,1e-4);
else
    if alg == 2
        alfa = armijo(alfafun,alfafun_d,armijo_kappa,armijo_ro,armijo_eps);
    else
        alfa = fminbnd(alfafun,alfa0,alfamax);
    end
end
x_prev= x_;
x_ = x_ + alfa*d_;
gr_prev = gr_k;

while (stop == 1 && norm(gr_k) > eps) || (stop == 2 && norm(d_)>eps) || (stop == 3 && norm(x_-x_prev)>eps)
    gr_k = df(x_);
    if showGRnorms == 1
        GR_norms = [GR_norms norm(gr_k)];
    end
    beta_k = dot(gr_k,gr_k) / dot(gr_prev, gr_prev); % F-R
    d_ = -gr_k + beta_k * d_;
    
    if alg == 1
        alfa = gold(alfafun,alfa0,alfamax,1e-4);
    else
        if alg == 2
            alfa = armijo(alfafun,alfafun_d,armijo_kappa,armijo_ro,armijo_eps);
        else
            alfa = fminbnd(alfafun,alfa0,alfamax);
        end
    end

    x_prev = x_;
    x_ = x_ + alfa*d_; 
    it = it + 1;
    gr_prev = gr_k;
end

xFR = x_;

end