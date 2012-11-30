# factor-graph 
This project's goal is to create a MATLAB/C++ framework for inference on Forney-style factor graphs.

Currently only gaussian distributions are supported.


# Factor graphs

TODO: general description

# Example (MATLAB)

The example considered will be a simple Kalman Filter (available in `examples/kalmanFilter.m`).

```
x_n = x_{n-1} + e
y = x_n + u
```

First, we need to create factor nodes themselves:

```Matlab
% input from the previous step, unobserved
xin = ffg.EvidenceNode;
%
xout = ffg.EvidenceNode;
% 
n = ffg.EvidenceNode;
% observed variable
y = ffg.EvidenceNode;
e = ffg.EqualityNode;
a = ffg.AddNode;
u = ffg.EvidenceNode;
b = ffg.AddNode;
```

Then, we need to connect them as the model requires. In this framework, we can do that with `ffg.Network`.

```Matlab
% creating the network itself
nwk = ffg.Network;
% connecting edges
nwk.addEdge(xin, e);
nwk.addEdge(e, b);
nwk.addEdge(u, b);
nwk.addEdge(b, xout);
nwk.addEdge(a, y);
nwk.addEdge(n, a);
```

Note, that each connection is directed. For each call `nwk.addEdge(a,b)`, underneath we 
are adding an _outgoing_ connection to _a_ and _incoming_ to _b_. If we want to specify
another type of connection (which nodes should support!), we could use `nwk.addEdgeTagged(a,b,tagForA,tagForB)`.

Now, that we have our network, we need to specify the order in which the messages are being
propagated (schedule). Simply create a cell array with pairs of nodes:

```Matlab
schedule = {xin, e, n, a, y, a, a, e, e, b, u, b, b, xout};
nwk.setSchedule(schedule);
```

Then, initialisation with dummy messages (Gauss messages can be created with `ffg.gaussMessage(mean,var,type)`.

```Matlab
xout.receive(ffg.gaussMessage(1+randn()*sd, sd^2, 'VARIANCE'));
n.receive(ffg.gaussMessage(0, sd2, 'VARIANCE'));
u.receive(ffg.gaussMessage(1.0, 0, 'VARIANCE'));
```

Finally, adaptive filtering, by observing yet another evidence (`y.receive`),
and passing unobserved estimation to the next step. To make a single passing step (single schedule pass),
we call `nwk.makeStep`.

```Matlab
for i = 1:N_ITERATIONS
  xin.receive(msg);
  y.receive(struct('mean', samples(i), 'var',0, 'type', 'VARIANCE'));
  nwk.makeStep();         
  msg = xout.evidence();
end
```


# Extension (MATLAB)
The framework provides basic means for extension. If you want to define a custom node
representing arbitrary table (for now only for Gaussian messages), you can use `ffg.CustomNode`.
This class has method `setFunction`, that takes the name of the function implementing 
your logic.

The function should has three parameters: 
* `from`- double - sender id
* `to` - double - receiver id
* `msgs` - array of structs with fields:
    - `mean` - mean vector
	- `var` - variance matrix
	- `connection` - the type of connection 'INCOMING'|'OUTGOING'|'estimate'
	- `from` - the sender of the message
	- `type` - the type of the message 'VARIANCE'|'PRECISION'


An example of such a function is `examples/customnode_function_gauss.m`

# Development
The major part of the C++ codebase has lots of comments. The examples of how to use it can be
found in `src/tests`.


# TODO
* Gradient descent
* EM algorithm
* Variational Bayes
* ? discrete variables
