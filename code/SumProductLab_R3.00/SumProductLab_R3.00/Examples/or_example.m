clear all
clc
disp('OR Example')
disp('   p  q  r')
disp('    \ | /')
disp('      s')
disp('      |')
disp('      t')
% instantiate nodes
p = evident_node(1);
q = evident_node(2);
r = evident_node(3);
s = or_node(4);
t = evident_node(5);
% define connections
p.setup_link({s});
q.setup_link({s});
r.setup_link({s});
s.setup_link({p q r t});   % parent parent child
t.setup_link({s});
% setup cpt for node r in the order of link list
%     P(r|p) P(r|q)
% cpt = [0.800 0.700];
% r.init_cp(cpt);
% % error check
% p.error_check();
% q.error_check();
% r.error_check();
% s.error_check();
% setup evidents and the nodes will interact accordingly
% setup both parents are untrue
p.setup_init_msg([0.9 0.1]);
q.setup_init_msg([0.9 0.1]);
r.setup_init_msg([0.9 0.1]);
t.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is false when all parents are false')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

% setup one parents is true
p.setup_init_msg([0.1 0.9]);
q.setup_init_msg([0.9 0.1]);
r.setup_init_msg([0.9 0.1]);
t.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is true when one of the parents is true')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

% setup child is true and two parents are false
p.setup_init_msg([0.9 0.1]);
q.setup_init_msg([0.9 0.1]);
r.setup_init_msg([0.5 0.5]);
t.setup_init_msg([0.1 0.9]);
% find marginals values of each link
disp('******** child is true and two parents are false make the last parent true')
disp(sprintf('link ps: %0.4f %0.4f',marginal(p,s)))
disp(sprintf('link qs: %0.4f %0.4f',marginal(q,s)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
disp(sprintf('link ts: %0.4f %0.4f',marginal(t,s)))

