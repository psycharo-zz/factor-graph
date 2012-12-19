%% Introduction
% We will see now how one can define a factor graph model and run
% sum-propagation algorithm in Matlab using the <matlab:doc('ffg') ffg framework>. We will
% use a specific type of linear-space model, known as Kalman Filter as
% an example. 
%
% The equation of the kalman filter is:
%
% $$X_t = X_{t-1} + U_t$
%
% $$Y_t = X_t + W_t$
% 
%
% And the factor graph will look as follows:
%
%             +---+ 
%             |U_t| 
%             +-+-+ 
%               | 
%               v 
% +-------+   +---+    +---+    +---+
% |X_{t-1}+-->|+,a+--->|=,e+--->|X_t| 
% +-------+   +---+    +-+-+    +---+ 
%                        | 
%                        v
%              +---+   +---+ 
%              |W_t+-->|+,b| 
%              +---+   +-+-+ 
%                        | 
%                        v 
%                      +---+ 
%                      |Y_t| 
%                      +---+ 
%

%% Creating the network
% The network in |ffg| framework is represented by a class ffg.Network. Let's create one:
% 
nwk = ffg.Network;
    
%% Creating nodes of the kalman filter
% Now, we need to add nodes and edges to this network. There are quite a
% few node types already available in the framework, including evidence
% nodes ffg.EvidenceNode, equality constaints nodes
% ffg.EqualityNode and zero-sum constraint nodes
% ffg.AddNode, all of them ancestor of a single base class
% ffg.FactorNode. Let's first create the nodes themselves:

% hidden variable, X_{t-1}
xin = ffg.EvidenceNode;
% hidden variable, X_t
xout = ffg.EvidenceNode;
% observation, corresponds to a half-edge y
y = ffg.EvidenceNode;
% equality node, connects a, b, xout
e = ffg.EqualityNode;
% process noise, U_t
u = ffg.EvidenceNode;
% observation noise, W_t
w = ffg.EvidenceNode;
% zero-sum node, connects xin + u = e
a = ffg.AddNode;
% zero-sum node, connects e + w = y
b = ffg.AddNode; 
%%%
% Every node has a unique identifier assigned when it is created, its
% value can be retrieved by the method |id()|.
%
%% Connecting the nodes
% Now we have the nodes, and we can actually connect them according to
% the model. This is done via the method |addEdge(src, dst)| of
% |ffg.Network|. Note, that by default each connection is directed. For
% each call |nwk.addEdge(src,dst)|, underneath we are adding an
% _outgoing_ connection to |src| and _incoming_ to |dst|:
% connecting nodes with each other
nwk.addEdge(xin, a); 
nwk.addEdge(u, a);
nwk.addEdge(a, e); 
nwk.addEdge(e, xout); 
nwk.addEdge(e, b);
nwk.addEdge(w, b); 
nwk.addEdge(b, y); 

%%%
% Some nodes, such as |ffg.EvidenceNode| and |ffg.EqualityNode| do not 
% distinguish between incoming and outgoing connections, so, in this 
% particular case |nwk.addEdge(e, xout)| and |nwk.addEdge(xout, e)| are equivalent.
% It is also possible, that a node might require other type of
% connection, rather than incoming or outgoing. In this case, we can
% specify the type of the connection by passing two more parameters to
% |ffg.addEdge|:
%%%
% 
% ffg.addEdge(src,dst,type_for_src, type_for_dst) 
%
%
% Where |type_for_src| is a string specifying the type
% of the connection for the node |src| (which, of course, *should
% support* this type of connection), and similarly |type_for_dst|. If
% the parameters are empty, the types are assumed to be |'outgoing'| for
% |src| and |'incoming'| for |dst|.
%
% That finishes the construction of the network. To have a look at it, we can use 
% the function |ffg.drawNetwork(nwk)|.
% drawing the constructed network
nwk.draw();

%% Creating the message-passing schedule
% Now we can use sum-propagation algorithm to do the actual filtering (in other words, 
% estimating unobserved variables). Here, we have two choices: 
%
% * use non-loopy summary propagation algorithm (works only for networks with no loops)
% * specify our own *schedule* of message propagation
% 
% Here we will consider only the latter one. The schedule is simply a cell array 
% of pairs of nodes; it can be specified for the network via |setSchedule| method:

nwk.setSchedule({xin, a; ...
                   u, a; ...
                   a, e; ...
                   y, b; ...
                   w, b; ...
                   b, e; ...
                   e, xout});

%% Initializing the network with evidence                   
% Before this schedule can be applied, we will want to initialise our model by
% setting some dummy values to evidence nodes (half-edges) - this is required
% by the summary propagation algorithm. To set an initial value to an instance
% of |ffg.EvidenceNode|, there is a method |receive(msg)|, that takes a message.
% Underneath, it just stores the message inside the nodes and sends it when
% the sum propagation algorithm is run. In this particular example, we need 
% to initialise nodes |u|, |w|, and |xout| (also |xin| and |y| but they 
% should receive updates on every iteration). Gaussian messages can be
% with |ffg.gaussVarianceMessage(mean,var)| or |ffg.gaussPrecisionMessage(mean,precision)|.
% For this example we will only need messages with variances:

% specifying some gaussian parameters               
sd = 10.0;
sd2 = sd*sd;
% initialising X_{t-1}
xout.receive(ffg.gaussVarianceMessage(1+randn()*sd, sd2));
% process noise, setting it to constant
u.receive(ffg.gaussVarianceMessage(1, 0));
% observation noise
w.receive(ffg.gaussVarianceMessage(0, sd2))

%%%    
% To see the list of messages for a node, one can use a method |messages|, that returns
% a cell array of messages. For instance for a node |w|, we |w.messages{1}| will 
% give the following result:
w.messages{1}

%%%
% Here, |from| specifies the sender of the message (|-1| means that there is no sender, 
% i.e. it corresponds to the half-edge), |type| is the type of the message (currently
% it is only 'VARIANCE' or 'PRECISION'), |mean| and |var| are specific for gaussians.


    
%% Filtering
% Finally, we can do the filtering. In this example, we use the current iteration
% number with the added noise as input data, and run sum propagation for a thousand times.
% On every step we initialise |xin| with an estimation of the previous step (dummy message 
% for the first iteration), and |y| by observing the new evidence (that we generated).
% To run a single iteration of the sum propagation algorithm on the network,
% we call |makeStep()| method of |ffg.Network|. To retrieve the message from |xout|
% we could, again, use the method |messages|, but than we would have to search through
% the cell array - |ffg.EvidenceNode| has a special method - |evidence()| that returns
% a message that it received from its connection (that was specified by |nwk.addEdge|).

% the number of timeslices to run
N_ITERATIONS = 1000;
% place to store results for each 
result = zeros(N_ITERATIONS, 2);    
samples = zeros(N_ITERATIONS, 1);
msg = ffg.gaussVarianceMessage(randn()*sd, sd2);
for i = 1:N_ITERATIONS
    samples(i) = i+randn()*sd;

    % setting X_{t-1} to the estimation on the previous step
    xin.receive(msg);
    % receiving subsequent observation
    y.receive(ffg.gaussVarianceMessage(samples(i), 0));
    nwk.makeStep();         
    msg = xout.evidence();

    result(i,:) = [msg.mean, msg.var];
end
    
%%% displaying results    
% plotting X estimations and the real 
subplot(2,1,1);
hold on;
plot(1:N_ITERATIONS,result(:,1), 'Color', 'b');
plot(1:N_ITERATIONS,samples(:,1), 'Color', 'r');
hold off;

title('X value');
subplot(2,1,2);
plot(1:N_ITERATIONS,result(:,2));
title('variance');


