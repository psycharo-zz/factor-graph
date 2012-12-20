function test_suite = testDynamicNetwork
    initTestSuite;


function testAdjacencyTemporal

    nwk = ffg.DynamicNetwork;

    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    node = ffg.AddNode(nwk);
    
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    nwk.addTemporalEdge(a, b);
    
    EXPECTED_MATRIX = zeros(4,4);
    EXPECTED_MATRIX(1,2) = 1;
    
    assertElementsAlmostEqual(nwk.adjacencyMatrixTemporal(), EXPECTED_MATRIX);


function testOverallKalmanFilter
% testing whether the dynamic network works in general
    nwk = ffg.DynamicNetwork;

    xin = ffg.EvidenceNode(nwk);
    xout = ffg.EvidenceNode(nwk);
    n = ffg.EvidenceNode(nwk);
    y = ffg.EvidenceNode(nwk);
    e = ffg.EqualityNode(nwk);
    a = ffg.AddNode(nwk);
    u = ffg.EvidenceNode(nwk);
    b = ffg.AddNode(nwk);
    
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
    
    xout.receive(ffg.messages.gaussVariance(1+randn()*sd, sd2));
    n.receive(ffg.messages.gaussVariance(0, sd2));
    u.receive(ffg.messages.gaussVariance(u_const, 0));
    xin.receive(ffg.messages.gaussVariance(1+randn()*sd, sd2));

    nwk.draw(3);

    NUM_ITERATIONS = 1000;
    results(1:NUM_ITERATIONS) = struct('id', [], 'message', []);
    for i = 1:NUM_ITERATIONS
        results(i) = struct('id', y.id, 'message', ffg.messages.gaussVariance(i+randn()*sd, 0));
    end
    nwk.makeStep(results, 1);
    
    EXPECTED_MEAN = NUM_ITERATIONS;
    EXPECTED_VAR = 1e-1;
    
    assertElementsAlmostEqual(EXPECTED_MEAN, xout.evidence().mean, 'absolute', 2);
    assertElementsAlmostEqual(EXPECTED_VAR, xout.evidence().var, 'absolute', 1e-1);
    
