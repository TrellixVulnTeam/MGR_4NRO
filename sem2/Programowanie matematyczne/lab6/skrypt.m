clc;
clear;
err1=[];
err2=[];
iters0=[];
iters1=[];
iters2=[];
wkts1=[];
wkts2=[];
epss=[];
m = 5;
n = 10;
eps=1e-6;
%for n=10:1:100
%for m=5:50
%for i=1:20
%eps=eps*2;
lower = -5;
upper = 5;
Losowanie;

f = zeros(1,n);
H = 2*diag(ones(1,n));
options = optimoptions('quadprog');
options = optimoptions(options,'Display', 'none');
[res,~,~,output,lambda] = quadprog(H,f,[],[],A,b,[],[],[],options);
iters0 = [iters0 output.iterations];
wkt1 = norm(abs((H*res)'+lambda.eqlin'*A));
wkt2 = norm(abs(A*res-b));
wkts1 = [wkts1 wkt1];
wkts2 = [wkts2 wkt2];

x0=zeros(n,1);
[x_k_1,ex_1,it_1] = ZFK(A,b,H,f,x0,0.001,1.1,eps,1);
[x_k_2,ex_2,it_2] = ZFK(A,b,H,f,x0,0.001,1.1,eps,2);

err1 =[err1 norm(res - x_k_1)];
err2 =[err2 norm(res - x_k_2)];
iters1 = [iters1 it_1];
iters2 = [iters2 it_2];
epss=[epss eps];

%end

%% porównanie iteracji quadprog i ZFK (od n)
% figure
% plot(10:100,iters0,'r',10:100,iters1,'g')
% title('Liczba iteracji w zależności od n')
% xlabel('n')
% ylabel('Liczba iteracji')
% legend('quadprog','ZFK')

%% porównanie błędu quadprog i ZFK (od n)
% figure
% plot(10:100,err1)
% title('Błąd w zależności od n (eps = 1e-6)')
% xlabel('n')
% ylabel('Błąd')

%% porównanie iteracji quadprog i ZFK (od m)
% figure
% plot(5:50,iters0,'r',5:50,iters1,'g')
% title('Liczba iteracji w zależności od m')
% xlabel('m')
% ylabel('Liczba iteracji')
% legend('quadprog','ZFK')

%% porównanie błędu quadprog i ZFK (od m)
% figure
% plot(5:50,err1)
% title('Błąd w zależności od m (eps = 1e-6)')
% xlabel('m')
% ylabel('Błąd')

%% porównanie iteracji quadprog i ZFK (od eps)
% figure
% semilogx(epss,iters0,'r',epss,iters1,'g')
% title('Liczba iteracji w zależności od eps')
% xlabel('eps')
% ylabel('Liczba iteracji')
% legend('quadprog','ZFK')

%% porównanie błędu quadprog i ZFK (od eps)
% figure
% loglog(epss,err1)
% title('Błąd w zależności od eps')
% xlabel('eps')
% ylabel('Błąd')

%% porównanie iteracji ZFK z fminunc i FR (od n)
% figure
% plot(10:100,iters1,'r',10:100,iters2,'g--')
% title('Liczba iteracji w zależności od n')
% xlabel('n')
% ylabel('Liczba iteracji')
% legend('fminunc','FR')

%% porównanie błędu quadprog i ZFK (od n)
% figure
% plot(10:100,err1,'r',10:100,err2,'g--')
% title('Błąd w zależności od n (eps = 1e-6)')
% xlabel('n')
% ylabel('Błąd')
% legend('fminunc','FR')