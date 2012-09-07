clear all
clc
disp('Noisy Or Example')
disp('    p   q')
disp('    \   /')
disp('      r')
disp('      |')
disp('      s')
% instantiate nodes
p = evident_node(1);
q = evident_node(2);
r = noisy_or_node(3);
s = evident_node(4);
% define connections
p.setup_link({r});
q.setup_link({r});
r.setup_link({p q s});   % parent parent child
s.setup_link({r});
% setup cpt for node r in the order of link list
%     P(r|p) P(r|q)
cpt = [0.800 0.700];
r.init_cp(cpt);
% error check
p.error_check();
q.error_check();
r.error_check();
s.error_check();
% setup evidents and the nodes will interact accordingly
% setup both parents are untrue
p.setup_init_msg([0.9 0.1]);
q.setup_init_msg([0.9 0.1]);
s.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is false when both parents are false')
disp(sprintf('link pr: %0.4f %0.4f',marginal(p,r)))
disp(sprintf('link qr: %0.4f %0.4f',marginal(q,r)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))

% setup one parents is true
p.setup_init_msg([0.1 0.9]);
q.setup_init_msg([0.9 0.1]);
s.setup_init_msg([0.5 0.5]);
% find marginals values of each link
disp('******** child is true when one of the parents is true')
disp(sprintf('link pr: %0.4f %0.4f',marginal(p,r)))
disp(sprintf('link qr: %0.4f %0.4f',marginal(q,r)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))

% setup chile is true and one parent is false
p.setup_init_msg([0.9 0.1]);
q.setup_init_msg([0.5 0.5]);
s.setup_init_msg([0 1]);
% find marginals values of each link
disp('******** chile is true and one parent is false make the other parent true')
disp(sprintf('link pr: %0.4f %0.4f',marginal(p,r)))
disp(sprintf('link qr: %0.4f %0.4f',marginal(q,r)))
disp(sprintf('link rs: %0.4f %0.4f',marginal(r,s)))
