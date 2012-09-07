clear all
clc
disp('Predict Hypothesis, h, from Evidence e1, e2, e3')
disp(' ')
disp('    h')
disp('    |')
disp('    x')
disp(' /  |  \')
disp('p1  p1  p3')
disp('|   |   |')
disp('e1  e2  e3')
disp('p1..p3 - cpt for evident e1..e3')
disp('h consists of 4 hypotheses')
disp('e1 has three alternatives')
disp('e2 has three alternatives')
disp('e3 has two alternatives')
% create nodes
h = evident_node(1);    % evident of hypothesis
x = equ_node(2);             % branching to evidents
p1 = cp2_node(3);            % cpt for E1
p2 = cp2_node(4);            % cpt for E2
p3 = cp2_node(5);            % cpt for E3
e1 = evident_node(6);   % evident for alarm1
e2 = evident_node(7);   % evident for alarm2
e3 = evident_node(8);   % evident for alarm3
% define connections
h.setup_link({x});
x.setup_link({h p1 p2 p3});
p1.setup_link({x e1});  % parent child
p2.setup_link({x e2});  % parent child
p3.setup_link({x e3});  % parent child
e1.setup_link({p1});
e2.setup_link({p2});
e3.setup_link({p3});
% set cpt for p1
%    x  e1    P(e1|x)
cpt( 1, 1) = 0.5;
cpt( 1, 2) = 0.4;
cpt( 1, 3) = 0.1;
cpt( 2, 1) = 0.06;
cpt( 2, 2) = 0.5;
cpt( 2, 3) = 0.44;
cpt( 3, 1) = 0.5;
cpt( 3, 2) = 0.1;
cpt( 3, 3) = 0.4;
cpt( 4, 1) = 1.0;
cpt( 4, 2) = 0;
cpt( 4, 3) = 0;
p1.init_cp(cpt);
% set cpt for p2
%    x  e2    P(e2|x)
cpt( 1, 1) = 0.8;
cpt( 1, 2) = 0.1;
cpt( 1, 3) = 0.1;
cpt( 2, 1) = 0.8;
cpt( 2, 2) = 0.1;
cpt( 2, 3) = 0.1;
cpt( 3, 1) = 0.1;
cpt( 3, 2) = 0.1;
cpt( 3, 3) = 0.8;
cpt( 4, 1) = 0.9;
cpt( 4, 2) = 0.05;
cpt( 4, 3) = 0.05;
p2.init_cp(cpt);
% set cpt for p3
%    x  e3    P(e3|x)
cpt( 1, 1) = 0.1;
cpt( 1, 2) = 0.9;
cpt( 2, 1) = 0.9;
cpt( 2, 2) = 0.1;
cpt( 3, 1) = 0.9;
cpt( 3, 2) = 0.1;
cpt( 4, 1) = 1.0;
cpt( 4, 2) = 0;
p3.init_cp(cpt);
% error checking
h.error_check();
x.error_check();
p1.error_check();
p2.error_check();
p3.error_check();
e1.error_check();
e2.error_check();
e3.error_check();
% setup evidents
h.setup_init_msg([0.5 0.5 0.5 0.5]);
e3.setup_init_msg([0.5 0.5]);
% case 1
e1.setup_init_msg([1 0 0]);
e2.setup_init_msg([0 0 1]);
% find marginals values of each link
disp('************************')
disp('Given e1 = [1 0 0]; e2 = [0 0 1]; e3 = [0.5 0.5];')
disp(sprintf('posterior prob of h: %0.4f %0.4f %0.4f %0.4f',marginal(h,x)))
% case 2
e1.setup_init_msg([0 0.5 0.5]);
e2.setup_init_msg([0.95 0.05 0]);
disp('************************')
disp('Given e1 = [0 0.5 0.5]; e2 = [0.95 0.05 0]; e3 = [0.5 0.5];')
disp(sprintf('posterior prob of h: %0.4f %0.4f %0.4f %0.4f',marginal(h,x)))
