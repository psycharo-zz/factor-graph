function p = marginal(m,n)
% calculate the summary or magin of link between two factor nodes
p = m.outbound_msg{find(m.to_id == n.id)}.*n.outbound_msg{find(n.to_id == m.id)};
p = p/sum(p);