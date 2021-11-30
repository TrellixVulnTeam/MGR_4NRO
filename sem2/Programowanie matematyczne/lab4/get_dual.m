function [c2,A2,b2] = get_dual(c,A,b,ub)
    [c,A,b] = convert_task(c,A,b,ub);
    c2 = b;
    A2 = A.';
    b2 = c;
end

function [c, A, b] = convert_task(c, A, b, ub)
    A = [A; diag(-ones(1,5)); diag(ones(1,5))];
    b = transpose([b ub ub]);
end

