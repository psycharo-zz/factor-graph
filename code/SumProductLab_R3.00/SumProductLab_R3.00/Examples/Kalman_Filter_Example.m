% Kalman Filter Example
clear all
clc
% The problem:
% To infere the internal state and how it evolve over time by observing
% from a noisy output with standard deviation sd.

% The solution:
% Find xout by forward propagation of message in the factor graph.  A
% Kalman filter has the following general form:
% xout = A*xin + B*u
% y = C*xin + n
% In this example, A, B, C are 1

% The factor graph:
%                u
%         (e)    |
%  xin --> = --> + --> xout
%          |    (b)
%     n--> +(a)
%          |
%          y
%  =====================>  direction of message propagation

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
sd = 10;
u_const = 1;
% first message
msg = [1+randn()*sd sd^2];
% inject dummy message at the end of the signal chain for completeness
xout.setup_init_msg(msg);
% noise source to n
n.setup_init_msg([0 sd^2]);
% constant to u
u.setup_init_msg([u_const 0]);

iteration = 100;
result = zeros(iteration,2);    % buffer to hold xout

% forward propagation
for i = 1:iteration
    % signal to xin
    xin.setup_init_msg(msg);
    % observable value to y
    y.setup_init_msg([i+randn()*sd 0]);
    % retrieve message from xout
    msg = xout.inbound_msg{1};
    result(i,:) = msg;
end

subplot(2,1,1)
plot(1:iteration,result(:,1));
title('xout')
subplot(2,1,2)
plot(1:iteration,result(:,2));
title('var')
