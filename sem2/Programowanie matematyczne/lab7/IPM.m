function [res,fval,it,LL_eqlin,LL_lower,ro, gamma, sigma,exitflag] = IPM(D,c,A,b,x0,y0,z0,eps1)

m = 1;
n = size(D,1);
x=x0;
y=y0;
z=z0;
delta = 0.2;
it=0;
MAX_IT=3000;
M=1e12;
exitflag=-1;
while it<MAX_IT
    
        if (max(abs(x)) > M)
            disp('zadanie prymarne jest nieograniczone')
            exitflag = -2;
            break;
        end
        if(max(abs(y)) > M)
            disp('zadanie dualne jest nieograniczone')
            exitflag = -3;
            break;
        end

    r=delta*(z'*x)/(n+m); %w12/s16
    delta=delta*0.8;
    [dx,dy,dz]=direction(x,y,z,A,b,D,c,r);
    alpha = alpha_f(x,dx,z,dz);
       
    x=x+alpha*dx;
    y=y+alpha*dy;
    z=z+alpha*dz;
    it = it+1;

    %w12/s17
    ro = b-A*x;
    gamma = z' * x;
    %w12/s26
    sigma = c-A'*y-z+D*x;
    

    if(sum(abs(ro))<eps1 && sum(abs(sigma))<eps1 && gamma<eps1)
        exitflag=1;
        break;
    end
end

res = x;
fval=0.5*x'*D*x+c'*x;
LL_eqlin = -y;
LL_lower= z;

end

%znika Y^-1 i W
function [dx,dy,dz] = direction(x,y,z,A,b,D,c,r)
%całość w12/s26
X = diag(x);
X_inv = diag(1./x);
Z=diag(z);

M1=[-X_inv*Z-D, A'; A,zeros(1,size(A,1))];
M2=[c-A'*y-r*X_inv*ones(size(X,1),1)+D*x; b-A*x];%znika Y^-1, bo nie ma W

dd=M1\M2;
dx = dd(1:size(A',1));
dy = dd((size(A',1)+1):size(dd,1));
e=ones(size(dx,1),1);
dz = X_inv*(r*e-X*Z*e-Z*dx);

end

function [alpha] = alpha_f(x,dx,z,dz)
%całość w12/s18
    beta = 0.9999;
   
    v_x = -beta *x./dx;
    v_x(dx>=0)=Inf;
    min_x = min(v_x); 

    v_z = -beta *z./dz;
    v_z(dz>=0)=Inf;
    min_z = min(v_z); 

    alpha = min([1,min_x,min_z]);
end