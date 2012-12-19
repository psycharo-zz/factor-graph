function test_suite = testDynamicNetwork
    initTestSuite;


function testAdjacencyTemporal

    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;
    node = ffg.AddNode;
    
    nwk = ffg.DynamicNetwork;
    
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    nwk.addTemporalEdge(a, b);
    
    EXPECTED_MATRIX = zeros(4,4);
    EXPECTED_MATRIX(1,2) = 1;
    
    assertElementsAlmostEqual(nwk.adjacencyMatrixTemporal(), EXPECTED_MATRIX);


function testOverallKalmanFilter
% testing whether the dynamic network works in general
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

    nwk.draw(3);

    NUM_ITERATIONS = 1000;
    results = struct('id', {}, 'message', {});
    for i = 1:NUM_ITERATIONS
        results = [results struct('id', y.id, 'message', ffg.gaussMessage(i+randn()*sd, 0, 'VARIANCE'))];
    end
    nwk.makeStep(results, 1);
    
    EXPECTED_MEAN = NUM_ITERATIONS;
    EXPECTED_VAR = 1e-1;
    
    assertElementsAlmostEqual(EXPECTED_MEAN, xout.evidence().mean, 'absolute', 2);
    assertElementsAlmostEqual(EXPECTED_VAR, xout.evidence().var, 'absolute', 1e-1);
    
