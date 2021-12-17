function [res_f,res_df,res_ddf] = fun(A,b)
    res_f = @(x)(0.5*x'*A*x-b'*x);
    res_df = @(x)(A*x-b);
    res_ddf = @(x)(A);
end