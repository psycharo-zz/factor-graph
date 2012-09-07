clear all
clc
disp('Explained Away Example')
disp('b - battery charged')
disp('f - fuel tank is full')
disp('g - gauge indicate full')
disp(' ')
disp('  b     f')
disp('   \   /')
disp('     m')
disp('     |')
disp('     g')
% instantiate nodes
b = evident_node(3);
f = evident_node(4);
m = cp3_node(5);
g = evident_node(6);
% define connections
b.setup_link({m});      % parent
f.setup_link({m});      % parent
m.setup_link({b f g});  % parent parent child
g.setup_link({m});      % child
% setup cpt for node n in the order of link list
%    b,f,g    P(g|b,f)
cptm(1,1,1) = 0.99;
cptm(1,1,2) = 0.01;
cptm(2,1,1) = 0.8;
cptm(2,1,2) = 0.2;
cptm(1,2,1) = 0.9;
cptm(1,2,2) = 0.1;
cptm(2,2,1) = 0.1;
cptm(2,2,2) = 0.9;
m.init_cp(cptm);
% setup evidents and the nodes will interact accordingly
b.setup_init_msg([0.1 0.9]);
f.setup_init_msg([0.1 0.9]);
g.setup_init_msg([1 0]);
% find marginals values of each link
disp('************************')
disp('Fuel gauge indicate empty')
disp(sprintf('marginal probability bm: %0.4f %0.4f',marginal(b,m)))
disp(sprintf('marginal probability fm: %0.4f %0.4f',marginal(f,m)))
disp(sprintf('marginal probability mg: %0.4f %0.4f',marginal(m,g)))
disp('************************')
disp('fuel gauge indicate empty')
disp('now we find battary is flat')
b.setup_init_msg([1 0]);
disp(sprintf('marginal probability bm: %0.4f %0.4f',marginal(b,m)))
disp(sprintf('marginal probability fm: %0.4f %0.4f',marginal(f,m)))
disp(sprintf('marginal probability mg: %0.4f %0.4f',marginal(m,g)))
disp('Fuel tank empty is explained away!')


