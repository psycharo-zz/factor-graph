function dynamicNetworkExample
%DYNAMICNETWORKEXAMPLE example of using DynamicNetwork for filtering

    xin = ffg.EvidenceNode;
    xout = ffg.EvidenceNode;
    n = ffg.EvidenceNode;
    y = ffg.EvidenceNode;
    e = ffg.EqualityNode;
    a = ffg.AddNode;
    u = ffg.EvidenceNode;
    b = ffg.AddNode;
    
    nwk = ffg.DynamicNetwork;
   
    nwk.addEdge(xin, e);
    nwk.addEdge(e, b);
    nwk.addEdge(u, b);
    nwk.addEdge(b, xout);
    nwk.addEdge(e, a);
    nwk.addEdge(a, y);
    nwk.addEdge(n, a);
    
    nwk.addTemporalEdge(xout, xin);
    
    schedule = {xin, e; ...
                n, a; ...
                y, a; ...
                a, e; ...
                e, b; ...
                u, b; ...
                b, xout};
    
    nwk.setSchedule(schedule);
    
    sd = 10.0;
    sd2 = sd*sd;
    u_const = 1.0;
    
    xout.receive(ffg.gaussMessage(1+randn()*sd, sd2, 'VARIANCE'));
    n.receive(ffg.gaussMessage(0, sd2, 'VARIANCE'));
    u.receive(ffg.gaussMessage(u_const, 0,'VARIANCE'));
    xin.receive(ffg.gaussMessage(1+randn()*sd, sd2, 'VARIANCE'));

    ffg.drawNetwork(nwk, 3);

    results = struct('id', {}, 'message', {});
    for i = 1:1000
        results = [results struct('id', y.id, 'message', ffg.gaussMessage(i+randn()*sd, 0, 'VARIANCE'))];
    end
    nwk.makeStep(results, 1);
    
    xout.evidence()
    

end

