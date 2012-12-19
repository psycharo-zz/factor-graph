%% example of using DynamicNetwork for filtering

%% Creating network

nwk = ffg.DynamicNetwork;

%
xin = ffg.EvidenceNode;
xout = ffg.EvidenceNode;
n = ffg.EvidenceNode;
y = ffg.EvidenceNode;
e = ffg.EqualityNode;
a = ffg.AddNode;
u = ffg.EvidenceNode;
b = ffg.AddNode;



nwk.addEdge(xin, e);
nwk.addEdge(e, b);
nwk.addEdge(u, b);
nwk.addEdge(b, xout);
nwk.addEdge(e, a);
nwk.addEdge(a, y);
nwk.addEdge(n, a);

%% Creating temporal connections
nwk.addTemporalEdge(xout, xin);

%% Setting schedule
schedule = {xin, e; ...
            n, a; ...
            y, a; ...
            a, e; ...
            e, b; ...
            u, b; ...
            b, xout};

nwk.setSchedule(schedule);

%% Initialisation with evidence
sd = 10.0;
sd2 = sd*sd;
u_const = 1.0;

xout.receive(ffg.gaussVarianceMessage(1+randn()*sd, sd2));
n.receive(ffg.gaussVarianceMessage(0, sd2));
u.receive(ffg.gaussVarianceMessage(u_const, 0));
xin.receive(ffg.gaussVarianceMessage(1+randn()*sd, sd2));

%% Drawing the network
nwk.draw(3);

%% Filling the input data
results = repmat(struct('id', {}, 'message', {}), 1, 1000);
for i = 1:1000
    results(i) = struct('id', y.id, 'message', ffg.gaussVarianceMessage(i+randn()*sd, 0));
end

%% Running sum-propagation
nwk.makeStep(results, 1);

%% Getting the estimation on i-th step
xout.evidence()



