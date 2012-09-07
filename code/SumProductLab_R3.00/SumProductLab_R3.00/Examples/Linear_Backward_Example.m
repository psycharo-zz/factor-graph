% Linear Backwork Propagation Example
clear all
clc
% The problem:
% There is a hidden sequence x0, x0+u, x0+2u,...
% where u is a known constant.  The observable values are superimposed with
% gaussian noise (0,sd)

% The objective:
% Find x0 by backward propagation of message in the factor graph.
% Also, performance of the factor graph is compared against linear
% regression.

% The factor graph:
%                u
%         (e)    |
%  xin --> = --> + --> xout
%          |    (b)
%     n--> +(a)
%          |
%          y
%  <=====================  direction of message propagation

% instantiate nodes
xin = evident_node(1);      % hidden state
xout = evident_node(2);     % next hidden state
n = evident_node(3);        % observation noise
y = evident_node(4);        % observed output
e = ls_equ_node(5);        % equ
a = ls_add_node(6);        % add, noise source
u = evident_node(7);        % input, constant
b = ls_add_node(8);        % add, input

% connect nodes
xin.setup_link({e});
b.setup_link({e u xout});   % xout is child, must be the last element
b.setup_operation([1 1]);
xout.setup_link({b});
n.setup_link({a});
y.setup_link({a});
e.setup_link({xin a b});
a.setup_link({e n y});      % y is child, must be at the last element
a.setup_operation([1 1]);
u.setup_link({b});

% parameters
u_const = 20;
sd = 2;
% preparing first test data
hidden_state = 15:u_const:100;  % x0 = 15
test_data = round(sd*randn(1,5) + hidden_state);

% backward propagation from xout to xin
msg = [test_data(end) sd^2];
% inject dummy message at the end of the signal chain for completeness
xin.setup_init_msg(msg);
% noise source to n
n.setup_init_msg([0 sd^2]);
% constant to u
u.setup_init_msg([u_const 0]);
% set up number of iteration in the factor graph section
% it is equal to size of test data array - 1
iteration = size(test_data,2) - 1;

x0 = zeros(1,100);          % buffer for x0 by backward propagation
lin_fit = zeros(1,100);     % buffer for x0 by linear regression

% run 100 times to get mean and var of x0
% and compare the performance of the factor graph vs linear regression

for k = 1:100
    for i = 1:iteration
        % signal to xout
        xout.setup_init_msg(msg);
        % observable value to y
        y.setup_init_msg([test_data(iteration-i+1) 0]);
        % retrieve message from xin
        msg = xin.inbound_msg{1};
    end
    % collect data from factor graph
    x0(k) = msg(1);
    % collect data from linear regression
    p = polyfit(1:size(test_data,2),test_data,1);
    lin_fit(k) = polyval(p,1);
    % generate new test data for next run
    test_data = round(sd*randn(1,5) + hidden_state);
    msg = [test_data(end) sd^2];
end

disp(sprintf('x0 mean and var by factor graph: %f, %f',mean(x0),var(x0)))
disp(sprintf('x0 mean and var by linear regression: %f, %f',mean(lin_fit),var(lin_fit)))


