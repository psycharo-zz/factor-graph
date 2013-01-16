function test_suite = testAddNode()
%TESTADDNODE testing ffg.AddNode
initTestSuite;

function testForward
%TESTFORWARD case for forward message (the result knowing all summands)

    nwk = ffg.Network;
    node = ffg.AddNode(nwk);
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    msgA = ffg.messages.gaussVariance(10, 5);
    msgB = ffg.messages.gaussVariance(20, 4);
    
    a.propagate(msgA);
    b.propagate(msgB);

    EXPECTED_MEAN_C = msgA.mean + msgB.mean;
    EXPECTED_VAR_C = msgA.var + msgB.var;

    msg = c.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_C);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_C);



function testBackward
%TESTFORWARD case for backward message (one of the summands knowing result)

    nwk = ffg.Network;
    node = ffg.AddNode(nwk);
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);

    
    msgC = ffg.messages.gaussVariance(10, 5);
    msgB = ffg.messages.gaussVariance(20, 4);
    
    c.propagate(msgC);
    b.propagate(msgB);

    EXPECTED_MEAN_A = msgC.mean - msgB.mean;
    EXPECTED_VAR_A = msgC.var + msgB.var;
    
    msg = a.evidence();
    assertEqual(size(msg.mean,2), 1);
    
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_A);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_A);
    
    
function testForwardPrecision
%TESTFORWARDPRECISION case for forward message (the result knowing all summands)

    nwk = ffg.Network;
    node = ffg.AddNode(nwk);
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    msgA = ffg.messages.gaussPrecision([10 10], [0.2  0; 0 0.2]);
    msgB = ffg.messages.gaussPrecision([20 20], [0.25 0; 0 0.25]);
    
    a.propagate(msgA);
    b.propagate(msgB);

    EXPECTED_MEAN_C = msgA.mean + msgB.mean;
    EXPECTED_PREC_C = msgA.precision * pinv(msgA.precision + msgB.precision) * msgB.precision;

    msg = c.evidence();
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_C);
    assertElementsAlmostEqual(msg.precision, EXPECTED_PREC_C);    

function testBackwardPrecision
%TESTFORWARDPRECISION case for backward message (the result knowing all summands)

    nwk = ffg.Network;
    node = ffg.AddNode(nwk);
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    msgA = ffg.messages.gaussPrecision(10, 1./5);
    msgC = ffg.messages.gaussPrecision(20, 1./4);
    
    a.propagate(msgA);
    c.propagate(msgC);

    EXPECTED_MEAN_B = msgC.mean - msgA.mean;
    EXPECTED_PREC_B = msgC.precision * pinv(msgC.precision + msgA.precision) * msgA.precision;

    msg = b.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_B);
    assertElementsAlmostEqual(msg.precision, EXPECTED_PREC_B); 
    
    
    


