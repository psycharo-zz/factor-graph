function connect(m,n)
% connect two factor nodes
% the 2nd node is append to the linklist of the 1st node
% the 1st node is prepend to the linklist of the 2nd node

m.append_link({n});
n.prepend_link({m});