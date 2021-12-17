function [A,b] = WygenerujMacierz(n,p1,p2)

A = randi([p1 p2],n,n);
A = A*A';

b = randi([min(A(:)) max(A(:))],n,1);

end