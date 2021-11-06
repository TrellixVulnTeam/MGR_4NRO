%dddd%
clc
clear
format
format compact

c = [1,1,1,1,1,1];
Aeq = [];
beq = [];
A = [1 0 0 0 0 1;
    1 1 0 0 0 0;
    0 1 1 0 0 0;
    0 0 1 1 0 0;
    0 0 0 1 1 0;
    0 0 0 0 1 1;];
b = [4;18;7;15;18;6];
lb = [0;0;0;0;0;0];
ub = [Inf;Inf;Inf;Inf;Inf;Inf];


opcje = optimset(@linprog);
opcje=optimset(opcje,'Display','iter','Algorithm','interior-point');

[x,fval,exitflag,output,lambda] = linprog(c,-A,-b,Aeq,beq,lb,ub,[],[])