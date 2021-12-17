function[x,retval,j,exitflag, x_primary] = compute_dual(c,A,b)
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
    [x,retval,j,exitflag,A2,retbase] = my_simplex(A,b',c',base,0);
    A2(:,base(ind)) = -A2(:,base(ind));
    x_primary = -c(retbase)' * A2(:,base);
end