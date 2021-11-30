function[] = compute_dual(c,A,b)
    neg = b<0
    [~,ind] = find(neg,10)
    b(ind)=-b(ind);
    A(ind,:) = -A(ind,:);
  


    %[x,retval,j,exitflag] = my_simplex(A,b',c',base,1)
end