function [xFR,fval,it, GR_norms] = FR(f,df,A,x0,eps)
GR_norms = [];

fval=0;

it = 0;
gr_k = df(x0);
d_ = -gr_k;
x_ = x0;
if norm(gr_k) <= eps
    xFR = x_;
    return;
end

x_prev= x_;
gr_prev = gr_k;

while norm(d_)>eps
    alfa = -(d_'*gr_k)/(d_'*A*d_);
    x_prev = x_;
    x_ = x_ + alfa*d_; 

    gr_k = df(x_);
    GR_norms = [GR_norms norm(gr_k)];
    beta_k = dot(gr_k,gr_k) / dot(gr_prev, gr_prev); % F-R
    d_ = -gr_k + beta_k * d_;
    
    
    it = it + 1;
    gr_prev = gr_k;

    if(norm(x_-x_prev)) < eps
        break;
    end
end

xFR = x_;
fval=f(x_);
end