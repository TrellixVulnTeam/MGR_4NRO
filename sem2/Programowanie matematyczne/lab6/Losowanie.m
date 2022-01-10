A = randi([lower,upper],m,n);
while rank(A) < m
    A = randi([lower,upper],m,n);
end
b = randi([lower,upper],m,1);
