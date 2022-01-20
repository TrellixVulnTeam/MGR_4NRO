function  rysuj3d(P,s,r)
% P - tablica wylosowanych punktów
% s - œrodek kuli w R3
% r - promieñ kuli
%%

[n,m]=size(P); %n=3, m-liczba wylosowanych punktów

if n==3
    
    hold on
    grid on
    axis square
    
    for i=1:m
        plot3(P(1,i),P(2,i),P(3,i),'r.');
    end

    plot3(s(1),s(2),s(3),'k*');
    
%     colormap gray
    [x,y,z]=sphere;
    h=surf(r*x+s(1),r*y+s(2),r*z+s(3));
    set(h,'FaceAlpha',0);
end

end

