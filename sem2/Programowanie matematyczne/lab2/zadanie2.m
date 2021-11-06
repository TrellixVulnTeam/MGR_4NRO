% 123
clc
clear
format
format compact

c = [-10;-8;2;9;-3]; %max
Aeq = [0 1 0 1 0];
beq = [10];
A = [-8 5 0 6 7;7 -7 10 -1 -9;7 3 3 -1 -8];
b = [2;1;2];
lb = [0;0;-Inf;-Inf;-Inf];
ub = [Inf;Inf;0;Inf;Inf];


opcje = optimset(@linprog);
opcje=optimset(opcje,'Display','iter','Algorithm','interior-point');

[x,fval,exitflag,output,lambda] = linprog(c,-A,-b,Aeq,beq,lb,ub,[],opcje)