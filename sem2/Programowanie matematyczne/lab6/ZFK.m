function [x_k,exitflag,i]=ZFK(A,b,H,ff,x0,r0,w,eps,version)
options = optimoptions('fminunc');
options = optimoptions(options, 'SpecifyObjectiveGradient', true, 'OptimalityTolerance', 1e-5,'StepTolerance',1e-5,'Display', 'none');

r_k = r0;
x_k = x0;

P = @(x,r) r*(A*x-b)'*(A*x-b);
dP = @(x,r) r*2*A'*(A*x-b);

f = @(x,r)x'*H*x/2 + ff*x + P(x,r); 
df = @(x,r)H*x - ff' +  dP(x,r);

f_df = @(x)deal(f(x,r_k),df(x,r_k));

i=0;
while P(x_k,r_k)>=eps && i<300

    if version == 1
        x_k = fminunc(f_df,x_k,options);
    else
        [x_k,~,~,~]=FR(@(x)f(x,r_k),@(x)df(x,r_k),H+2*r_k*(A'*A),x_k,eps);
    end
    r_k = w * r_k;
    f_df = @(x)deal(f(x,r_k),df(x,r_k));

i = i+1;
end

exitflag =1;
end