% 123
clc
clear
format
format compact

c = [2;3]; %max
A = [1 3; 1 -2; 6 5; 3 -1; 6 2];
b = [6;2;15;5;8];
lb = zeros(2,1);

opcje = optimset(@linprog);
opcje=optimset(opcje,'Display','iter','Algorithm','interior-point');

[x,fval,exitflag,output,lambda] = linprog(-c,A,b,[],[],lb,[],[],opcje)