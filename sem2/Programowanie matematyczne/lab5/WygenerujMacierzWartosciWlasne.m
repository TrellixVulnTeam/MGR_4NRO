function [A,b] = WygenerujMacierzWartosciWlasne(Q)

n = length(Q);
D = diag(Q);
V = rand(n,n);
while det(V)==0
    V = rand(n,n);
end
V = orth(V);
A = V*D*V';
b=rand(n,1);
min_ = min(A(:));
max_ = max(A(:));
b= (max_-min_) * b + min_;

end