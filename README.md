# factor-graph 
This project's goal is to create a MATLAB/C++ framework for inference on Forney-style factor graphs.

The latest package can be downloaded [here](https://github.com/psycharo/factor-graph/archive/master.zip).

Currently only gaussian distributions are supported.

More in the [Wiki](https://github.com/psycharo/factor-graph/wiki)


# Installation

Just unpack the .zip and run `installFFG.m`. This will build the framework and add `ffg` to matlab path. 
Should work on Matlab 2012b under Windows and Linux.

# Factor graphs intro

Factor graph is a specific type of graphical model used to represent
_factorizations_ of functions. They were originally developed to solve
coding and signal processing problems. However, their area of
application goes beyound that: such well known probabalistical
graphical models as Bayesian and Markov networks can be represented as
factor graphs (by Hammersley–Clifford theorem). The algorithms used
with factor graphs use the concept of _messages_, or _summaries_,
that are passed along graph's edges, and usually are refered to as
_summary propagation_ algorithms. We are focusing on a specific notation for factor graphs, called 
Forney-Style factor graphs (FFG). Below we give a short intro into the
concept. For more details, see [Loeliger2004](http://www.robots.ox.ac.uk/~parg/mlrg/papers/factorgraphs.pdf)

Let's start with an example. Let `f(w,x,y,z) = g(w,x,y) h(y,z)` be a
factorization of a function `f(w,x,y,z)` of several variables. The FFG
expressing this factorization will look as follows:

<pre>
            |
           x|
            |
     w    +-+-+    y    +---+    z
 ---------+ g +---------+ h +---------
          +---+         +---+
</pre>

As you can see, FFG consists of nodes (`g` and `h`), edges (`y`) and
half-edges (`w`, `x` and `z`). It can be constructed of a function
factorization according to the following rules:
* for each factor, there is a node in the graph
* for each variable appearing in two factors, there is an edge in the
  graph
* for each variable appearing in one factor, there is a half-edge in
  the graph

Note that factorizations are restricted to those with no more than two
factors sharing a variable. This restriction can be easily resolved
though, by introducing extra variables and an equality
constraint. Let's assume that we have factorization `f(x,y,z) =
g(x,y)h(y,z)l(y)`. The variable `y` is shared by three factors `g`,`h`
and `l`. Let's introduce variables `y'` and `y''`, and equality
constraint `y = y' = y''`. The factor that enables this equality
constraint is `f=(y,y',y'')=d(y - y')d(y - y'')`, where `d` is Dirac's
delta function. The FFG of the modified factorization will then look
as follows:

<pre>
   x   +---+    y    +---+    y'   +---+   z
-------+ g +---------+ = +---------+ h +-------
       +---+         +-+-+         +---+
                       |
                       |y''
                       |
                       |
                     +-+-+
                     | l |
                     +---+
</pre>

Another useful constraint factor is the _zero-sum constraint factor_:
`f+(x,y,z) = d(x - y - z)`, that expresses the constraint `x = y +
z`. This, of course, can be generalized further.

Factor graphs can be used to express a variety of models. A real world
yet simple example would be a linear-space model, a model widely used
in signal processing: 

```LaTeX
X_t = A X_{t−1} + B U_t
Y_t = C X_t + W_t
```

Here `Y_t` can be thought of as an observation (measurement) that is
made on a step `t`; `X_t` and `X_{t-1} are unobserved random variables
on steps `t` and `t-1` correspondinly; U_t is the process noise, W_t
is the observation noise (both assumed to be drawn from Gaussian
distribution); and A, B, C are parameters of the model (in the
simplest case, all identities).

To construct the graphical model in FFG notation, we can make use of
the equality constaint and zero-sum constraint factors defined above,
and also introduce a new factor to express matrix
multiplication. Similarly, `f_A(x, y) = d(y - A x)` will ensure
constraint `y = A x`. The whole model in FFG notation will then look
as follows:

<pre>
                      U_t
                       |
                       |
                       v
                     +---+
                     | B |
                     +-+-+
                       |
                       v
           +---+     +---+     +---+
 X_{t-1}-->| A +---->| + +---->| = +--->X_t
           +---+     +---+     +-+-+
                                 |
                                 v
                               +---+
                               | C |
                               +-+-+
                                 |
                                 v
                    +---+      +---+
                    |W_t+----->| + |
                    +---+      +-+-+
                                 |
                                 v
                                Y_t
</pre>

# Example, Kalman Filtering

We will see now how one can define a factor graph model and run
sum-propagation algorithm in Matlab using the `ffg` framework. We will
use a specific type of linear-space model, known as Kalman Filter as
an example (latest code for example is available in
[kalmanScheduleExample](https://github.com/psycharo/factor-graph/blob/master/examples/kalmanFilterExample.m):

```
X_t = X_{t-1} + U_t
Y_t = X_t + W_t
```

Let's first see how the FFG for a single timeslice will look like. In
the framework, for convenience, we substitute each _half-edge_ with an
_edge_ and a special kind of node, to which we will refer to as
`evidence node`. The factor graph for this model will be (for a
timeslice `t`, letters are used to refer them in the code later): 


<pre>
            +---+ 
            |U_t| 
            +-+-+ 
              | 
              v 
+-------+   +---+    +---+    +---+
|X_{t-1}+-->|+,a+--->|=,e+--->|X_t| 
+-------+   +---+    +-+-+    +---+ 
		       | 
		       v
             +---+   +---+ 
             |W_t+-->|+,b| 
             +---+   +-+-+ 
                       | 
                       v 
                     +---+ 
                     |Y_t| 
                     +---+ 
</pre>

The network in `ffg` framework is represented by a class `ffg.Network`. Let's create one:

```Matlab
nwk = ffg.Network;
```

Now, we need to add nodes and edges to this network. There are quite a
few node types already available in the framework, including evidence
nodes (`ffg.EvidenceNode`), equality constaints nodes
(`ffg.EqualityNode`) and zero-sum constraint nodes
(`ffg.AddNode`), all of them ancestor of a single base class
`ffg.FactorNode`. Let's first create the nodes themselves:

```Matlab
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
```

Every node has a unique identifier assigned when it is created, its
value can be retrieved by the method `id()`.


Now we have the nodes, and we can actually connect them according to
the model. This is done via the method `addEdge(src, dst)` of
`ffg.Network`. Note, that by default each connection is directed. For
each call `nwk.addEdge(src,dst)`, underneath we are adding an
_outgoing_ connection to `src` and _incoming_ to `dst`:

```Matlab
% connecting nodes with each other
nwk.addEdge(xin, a); 
nwk.addEdge(u, a);
nwk.addEdge(a, e); 
nwk.addEdge(e, xout); 
nwk.addEdge(e, b);
nwk.addEdge(w, b); 
nwk.addEdge(b, y); 
``` 

Some nodes, such as `ffg.EvidenceNode` and `ffg.EqualityNode` do not 
distinguish between incoming and outgoing connections, so, in this 
particular case `nwk.addEdge(e, xout)` and `nwk.addEdge(xout, e)` are equivalent.

It is also possible, that a node might require other type of
connection, rather than incoming or outgoing. In this case, we can
specify the type of the connection by passing two more parameters to
`ffg.addEdge`:

```Matlab 
ffg.addEdge(src,dst,type_for_src, type_for_dst) 
``` 

Where `type_for_src` is a string specifying the type
of the connection for the node `src` (which, of course, *should
support* this type of connection), and similarly `type_for_dst`. If
the parameters are empty, the types are assumed to be `'outgoing'` for
`src` and `'incoming'` for `dst`.

That finishes the construction of the network. To have a look at it, we can use 
the function `ffg.drawNetwork(nwk)`.


Now we can use sum-propagation algorithm to do the actual filtering (in other words, 
estimating unobserved variables). Here, we have two choices: 
* use non-loopy summary propagation algorithm (works only for networks with no loops)
* specify our own *schedule* of message propagation

Here we will consider only the latter one. The schedule is simply a cell array 
of pairs of nodes; it can be specified for the network via `setSchedule` method:

```Matlab
% creating message-passing schedule
nwk.setSchedule({xin, a; ...
                   u, a; ...
                   a, e; ...
                   y, b; ...
                   w, b; ...
                   b, e; ...
                   e, xout});
```

Before this schedule can be applied, we will want to initialise our model by
setting some dummy values to evidence nodes (half-edges) - this is required
by the summary propagation algorithm. To set an initial value to an instance
of `ffg.EvidenceNode`, there is a method `receive(msg)`, that takes a message.
Underneath, it just stores the message inside the nodes and sends it when
the sum propagation algorithm is run. In this particular example, we need 
to initialise nodes `u`, `w`, and `xout` (also `xin` and `y` but they 
should receive updates on every iteration). Gaussian messages can be
with `ffg.gaussVarianceMessage(mean,var)` or `ffg.gaussPrecisionMessage(mean,precision)`.
For this example we will only need messages with variances:


```Matlab
% specifying distribution params         
sd = 10.0;
sd2 = sd*sd;
% initialising X_{t-1}
xout.receive(ffg.gaussVarianceMessage(1+randn()*sd, sd2));
% process noise, setting it to constant
u.receive(ffg.gaussVarianceMessage(1, 0));
% observation noise
w.receive(ffg.gaussVarianceMessage(0, sd2));
```


To see the list of messages for a node, one can use a method `messages`, that returns
a cell array of messages. For instance for a node `w`, we `w.messages{1}` will 
give the following result:

```
ans = 
    from: -1
    type: 'VARIANCE'
    mean: 0
     var: 100
```

Here, `from` specifies the sender of the message (`-1` means that there is no sender, 
i.e. it corresponds to the half-edge), `type` is the type of the message (currently
it is only 'VARIANCE' or 'PRECISION'), `mean` and `var` are specific for gaussians.


Finally, we can do the filtering. In this example, we use the current iteration
number with the added noise as input data, and run sum propagation for a thousand times.
On every step we initialise `xin` with an estimation of the previous step (dummy message 
for the first iteration), and `y` by observing the new evidence (that we generated).
To run a single iteration of the sum propagation algorithm on the network,
we call `makeStep()` method of `ffg.Network`. To retrieve the message from `xout`
we could, again, use the method `messages`, but than we would have to search through
the cell array - `ffg.EvidenceNode` has a special method - `evidence()` that returns
a message that it received from its connection (that was specified by `nwk.addEdge`).


```Matlab
% the number of iterations to run sum-propagation
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
```





# TODO
* Gradient descent
* EM algorithm
* Variational Bayes
* ? discrete variables
