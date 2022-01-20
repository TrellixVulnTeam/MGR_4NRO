clc;
clear;
warning off MATLAB:singularMatrix;
warning off MATLAB:nearlySingularMatrix;
rng(1);
its1=[];
its2=[];
errs_cust = [];
errs_eqlin = [];
errs_lower = [];
errs_ro = [];
m_range=  10:5:400;

for m=m_range

P = rand(3,m)*400-200;

x0 = sum(P,2)/m; %"środek ciężkości" wylosowanych punktów
tmp = x0-P;
dists = sqrt(sum(tmp.*tmp,1)); %odległości od x0
r0 = max(dists); % promień kuli o środku w x0
z0 = r0*r0; % z definicji z
v_z0 = z0 - dists; % początkowy wektor 'z' (dopełnień do ograniczenia równościowego)
v_y0 = ones(m,1)/m; % początkowy wektor 'y' z ZP2 (sumuje się do 1);

H = 2 * (P'*P);
f = -sum(P.*P);

beq = 1;
Aeq = ones(1,m);
lb = zeros(m,1);
options = optimoptions('quadprog');
options = optimoptions(options,"Display","none","ConstraintTolerance",1e-10,"OptimalityTolerance",1e-10);
[res,fval,~,output,lambda] = quadprog(H,f,[],[],Aeq,beq,lb,[],v_y0,options);
it1 = output.iterations;
s1 = P * res;
r1 = sqrt(-fval);

%figure
%rysuj3d(P,s1,r1)

[res2,fval2,it2,eqlin,lower,ro,gamma,sigma,exitflag2] = IPM(H,f',Aeq,beq,v_y0,z0,v_z0',1e-6);

s2=P*res2;
r2=sqrt(-fval2);
%figure
%rysuj3d(P,s2,r2)
err_cust = norm(s1-s2)+norm(r1-r2);
errs_cust=[errs_cust err_cust];
errs_ro = [errs_ro norm(res-res2)];
errs_eqlin = [errs_eqlin norm(eqlin-lambda.eqlin)];
errs_lower = [errs_lower norm(lower-lambda.lower)];
its1=[its1 it1];
its2=[its2 it2];

end


figure
semilogy(m_range,errs_cust,'r',m_range,errs_ro,'g',m_range,errs_eqlin,'b',m_range,errs_lower,'c')
title('Błędy w zależności od m')
xlabel('m')
ylabel('Błąd')
legend('Błąd - customowy','Błąd RO', 'Błąd eqlin', 'Błąd lower')

figure
plot(m_range,its1,'r',m_range,its2,'g')
title('Liczba iteracji w zależności od m')
xlabel('m')
ylabel('Liczba iteracji')
legend('quadprog','IPM')