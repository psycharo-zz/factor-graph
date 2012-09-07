function p = ls_marginal(m,n)
% calculate the summary or magin of link between two factor nodes
x = m.outbound_msg{find(m.to_id == n.id)};
y = n.outbound_msg{find(n.to_id == m.id)};
if isfinite(x(2)) && isfinite(y(2))
    p_covar = x(2)*y(2)/(x(2) + y(2));
    p_mean = (y(2)*x(1) + x(2)*y(1))/(x(2) + y(2));
    p = [p_mean p_covar];
elseif isfinite(x(2)) && isinf(y(2))
    p_covar = x(2);
    p_mean = x(1);
    p = [p_mean p_covar];
elseif isinf(x(2)) && isfinite(y(2))
    p_covar = y(2);
    p_mean = y(1);
    p = [p_mean p_covar];
else
    %disp(sprintf('Marginal value undetermined from node %d %d',n.id, m.id))
    p = [x(1)+y(1) Inf];
end
