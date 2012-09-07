% Low-Density Parity-Check (LDPC) code example from wiki

clear all
clc

disp('LDPC Example from wiki:')
disp('http://en.wikipedia.org/wiki/Low-density_parity-check_code')
disp(' ')

global loopy
loopy = 1;

% instantiate nodes
e1 = evident_node(1);
e2 = evident_node(2);
e3 = evident_node(3);
e4 = evident_node(4);
e5 = evident_node(5);
e6 = evident_node(6);
d1 = equ_node(7);
d2 = equ_node(8);
d3 = equ_node(9);
d4 = equ_node(10);
d5 = equ_node(11);
d6 = equ_node(12);
x1 = even_parity_node(13);
x2 = even_parity_node(14);
x3 = even_parity_node(15);

% define connection
e1.setup_link({d1});
e2.setup_link({d2});
e3.setup_link({d3});
e4.setup_link({d4});
e5.setup_link({d5});
e6.setup_link({d6});
d1.setup_link({e1 x1 x3});
d2.setup_link({e2 x1});
d3.setup_link({e3 x1 x2});
d4.setup_link({e4 x1 x2 x3});
d5.setup_link({e5 x3});
d6.setup_link({e6 x2});
x1.setup_link({d1 d2 d3 d4});
x2.setup_link({d3 d4 d6});
x3.setup_link({d1 d4 d5});

% setup evidents
% e1.setup_init_msg([0.1 0.9]);
% e2.setup_init_msg([0.9 0.1]);
% e3.setup_init_msg([0.1 0.9]);
% e4.setup_init_msg([0.1 0.9]);
% e5.setup_init_msg([0.1 0.9]);
% e6.setup_init_msg([0.1 0.9]);

e1.setup_init_msg([0.9 0.1]);
e2.setup_init_msg([0.9 0.1]);
e3.setup_init_msg([0.1 0.9]);
e4.setup_init_msg([0.1 0.9]);
e5.setup_init_msg([0.1 0.9]);
e6.setup_init_msg([0.1 0.9]);

code = zeros(1,6);
code(1) = (e1.outbound_msg{1}(2) > 0.5);
code(2) = (e2.outbound_msg{1}(2) > 0.5);
code(3) = (e3.outbound_msg{1}(2) > 0.5);
code(4) = (e4.outbound_msg{1}(2) > 0.5);
code(5) = (e5.outbound_msg{1}(2) > 0.5);
code(6) = (e6.outbound_msg{1}(2) > 0.5);

disp('Initial Message:')
disp(code)

disp('Output of the LDCP network')
disp('------------------------------------------')
% iteration
for i = 1:10
    
    % update network
    d1.update_node();
    d2.update_node();
    d3.update_node();
    d4.update_node();
    d5.update_node();
    d6.update_node();
    x1.update_node();
    x2.update_node();
    x3.update_node();
    
    % result
    p = marginal(e1,d1);
    code(1) = p(2) > 0.5;
    p = marginal(e2,d2);
    code(2) = p(2) > 0.5;
    p = marginal(e3,d3);
    code(3) = p(2) > 0.5;
    p = marginal(e4,d4);
    code(4) = p(2) > 0.5;
    p = marginal(e5,d5);
    code(5) = p(2) > 0.5;
    p = marginal(e6,d6);
    code(6) = p(2) > 0.5;

    disp(code);
    
end
