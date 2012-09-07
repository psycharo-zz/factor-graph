clear all
clc
disp('******* and_node test *******')
% instantiate nodes
p = evident_node(1);
q = evident_node(2);
r = evident_node(3);
s = and_node(4);
t = evident_node(5);
% define connections
p.setup_link({s});
q.setup_link({s});
r.setup_link({s});
s.setup_link({p q r t});   % parent parent child
t.setup_link({s});
disp('parent nodes: p  q  r')
disp('              \  |  /')
disp('and_node:        s')
disp('                 |')
disp('child node:      t')
% setup evidents and the nodes will interact accordingly
% setup both parents are untrue
p.setup_init_msg([0.1 0.9]);
q.setup_init_msg([0.1 0.9]);
r.setup_init_msg([0.1 0.9]);
t.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is true when all parents are true')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

% setup one parents is false
p.setup_init_msg([0.9 0.1]);
q.setup_init_msg([0.1 0.9]);
r.setup_init_msg([0.1 0.9]);
t.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is false when one of the parents is false')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

% setup child is false and two parents are true
p.setup_init_msg([0.1 0.9]);
q.setup_init_msg([0.1 0.9]);
r.setup_init_msg([0.5 0.5]);
t.setup_init_msg([0.9 0.1]);
% find marginals values of each link
disp('******** child is false and two parents are true make the last parent true')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

