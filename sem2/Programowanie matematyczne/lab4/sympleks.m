function[ROx, ROy, exitflag] = sympleks(c,A,b,g)
    [c,A,b]=get_dual(c,A,b,g);
    c=-c;
    neg = b<0;
    [~,ind] = find(neg,10);
    b(ind)=-b(ind);
    A(ind,:) = -A(ind,:);
  
    base = zeros(1,5);

    for i=1:5
        if isempty(find(ind==i,1))
            base(i) = i+10;
        else
            base(i) = i+5;
        end
    end
    [ROy,retval,j,exitflag,A2,retbase] = my_simplex(A,b',c',base,1);
    A2(:,base(ind)) = -A2(:,base(ind));
    ROx = c(retbase)' * A2(:,base);
    ROy=ROy';
end