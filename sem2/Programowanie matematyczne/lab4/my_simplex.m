function [x, retval, j, exitflag, A2,retbase] = my_simplex(A, b, c, base, should_print)
    eps = -1e-10;
    j = 0;
    z = c(base) * A;
    z_c = z - c;
    exitflag=1;
    while any(z_c < eps) && j < 100
        if should_print
                res = zeros(length(c),1);
    res(base) = b;
    retval = c * res;
    fprintf("res = %0.3f\n",retval)
            print_simplex(j,A,b,c,base,z,z_c);
        end

        [~, ind] = min(z_c);
        
        b2 = b ./ (A(:,ind));
        b2(A(:,ind)<=0) = Inf;
        b2(b2<0) = Inf;
        b2(isnan(b2)) = Inf;
        [~, ind2] = min(b2);
        base(ind2) = ind;
    
        b(ind2) = b(ind2) / A(ind2,ind);
        A(ind2,:) = A(ind2,:) / A(ind2,ind);
        
    
        row_tmp = A(ind2,:);
        b_tmp = b(ind2);
        b = b - A(:,ind) * b_tmp;
        A = A - A(:,ind) * row_tmp;
        A(ind2,:) = row_tmp;
        b(ind2) = b_tmp;

        z = c(base) * A;
        z_c = z-c;

        j=j+1;
    end

    if should_print
        print_simplex(j,A,b,c,base,z,z_c);
    end
    res = zeros(length(c),1);
    res(base) = b;
    retval = c * res;
    x=res;
    A2 = A;
    retbase = base;
    if j<1000
        if should_print
            fprintf("x = [%0.3f, %0.3f, %0.3f, %0.3f, %0.3f]\n",x(1),x(2),x(3),x(4),x(5))
            fprintf("res = %0.3f\n",retval)
        end
    else
        exitflag=0;
        if should_print
            fprintf("Brak rozwiazan")
        end
    end
end

function [] = print_simplex(j,A,b,c,base,z,z_c)
    eps = 1e-4;
    fprintf("Step: %i\n", j);

    fprintf("-------------------------------------------------------------------------------------------------------------------- \n");
    fprintf("\n          c  ||");
    for j = 1:length(c)
        if abs(c(j)) < eps
            c(j) = 0;
        end
        if c(j)<0
            fprintf(" %0.3f ", c(j))
        else
            fprintf("  %0.3f ", c(j))
        end
    end 
    fprintf("\n")
    for i = 1:length(base)
        if base(i) < 10
            fprintf("x_%i  = ",base(i))
        else
            fprintf("x_%i = ",base(i))
        end
        if c(base(i)) < 10
            fprintf("%0.3f ||",c(base(i)))
        else
            fprintf("%0.3f||",c(base(i)))
        end
        for j = 1:length(c)
            if abs(A(i,j)) < eps
                A(i,j) = 0;
            end
            if A(i,j) < 0
                fprintf(" %0.3f ", A(i,j))
            else
                fprintf("  %0.3f ", A(i,j))
            end
        end
        fprintf("| %0.3f \n",b(i))
    end
    fprintf("          z  ||");
    for j = 1:length(c)
        if abs(z(j)) < eps
            z(j) = 0;
        end
        if z(j)<0
            fprintf(" %0.3f ", z(j))
        else
            fprintf("  %0.3f ", z(j))
        end
    end
    fprintf("\n         z-c ||");
    for j = 1:length(c)
        if abs(z_c(j)) < eps
            z_c(j) = 0;
        end
        if z_c(j)<0
            fprintf(" %0.3f ", z_c(j))
        else
            fprintf("  %0.3f ", z_c(j))
        end
    end
    fprintf("\n-------------------------------------------------------------------------------------------------------------------- \n");
    fprintf("-------------------------------------------------------------------------------------------------------------------- \n\n");
end
