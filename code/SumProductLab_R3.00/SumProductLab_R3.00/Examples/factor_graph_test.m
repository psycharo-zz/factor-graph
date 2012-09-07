clear all
clc
disp('Factor Graph Test')
disp(' ')
disp('   p     q')
disp('    \   /')
disp('      m')
disp('      |')
disp('      n')
disp('    /   \')
disp('   r     s')

% instantiate nodes
m = even_parity_node(1);
n = even_parity_node(2);
p = evident_node(3);
q = evident_node(4);
r = evident_node(5);
s = evident_node(6);
% define connections
m.setup_link({p q n});
n.setup_link({r s m});
p.setup_link({m});
q.setup_link({m});
r.setup_link({n});
s.setup_link({n});
% setup evidents and the nodes will interact accordingly
p.setup_init_msg([0.9 0.1]);    % 0
q.setup_init_msg([0.9 0.1]);    % 0
r.setup_init_msg([0.1 0.9]);    % 1
s.setup_init_msg([0.9 0.1]);    % 0
% error check
m.error_check();
n.error_check();
p.error_check();
q.error_check();
r.error_check();
s.error_check();
% find marginals values of each link
disp('Evidents:')
disp('p,q,r,s = 0,0,1,0')
disp(' ')
disp('Marginal values of each link:')
disp(sprintf('link mn: %0.4f %0.4f',marginal(m,n)))
disp(sprintf('link mp: %0.4f %0.4f',marginal(m,p)))
disp(sprintf('link mq: %0.4f %0.4f',marginal(m,q)))
disp(sprintf('link nr: %0.4f %0.4f',marginal(n,r)))
disp(sprintf('link ns: %0.4f %0.4f',marginal(n,s)))
disp(' ')
% update event
q.setup_init_msg([0.1 0.9]);    % 1
disp('Now change evidents of q to 1:')
disp('p,q,r,s = 0,1,1,0')
disp(' ')
% update marginals
disp('Revised marginal values of each link:')
disp(sprintf('link mn: %0.4f %0.4f',marginal(m,n)))
disp(sprintf('link mp: %0.4f %0.4f',marginal(m,p)))
disp(sprintf('link mq: %0.4f %0.4f',marginal(m,q)))
disp(sprintf('link nr: %0.4f %0.4f',marginal(n,r)))
disp(sprintf('link ns: %0.4f %0.4f',marginal(n,s)))

