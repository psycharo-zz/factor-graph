function test_suite = testCustomNode
%TESTCUSTOMNODE Summary of this function goes here
%   Detailed explanation goes here
initTestSuite;

function testAddForwardNode
    nwk = ffg.Network;
    node = ffg.CustomNode(nwk);
        
    node.setFunction('ffg.tests.customnode_function_gauss');
    
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    % input
    msgA = ffg.messages.gaussVariance(10, 5);
    msgB = ffg.messages.gaussVariance(20, 4);
    %
    
    a.propagate(msgA);
    b.propagate(msgB);

    EXPECTED_MEAN_C = msgA.mean + msgB.mean;
    EXPECTED_VAR_C = msgA.var + msgB.var;

    msg = c.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_C);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_C);
    
    
function testAddBackwardNode
    nwk = ffg.Network;
    node = ffg.CustomNode(nwk);
    
    node.setFunction('ffg.tests.customnode_function_gauss');
    
    a = ffg.EvidenceNode(nwk);
    b = ffg.EvidenceNode(nwk);
    c = ffg.EvidenceNode(nwk);
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);

    % input
    msgC = ffg.messages.gaussVariance(10, 5);
    msgB = ffg.messages.gaussVariance(20, 4);
    %
    
    c.propagate(msgC);
    b.propagate(msgB);

    EXPECTED_MEAN_A = msgC.mean - msgB.mean;
    EXPECTED_VAR_A = msgC.var + msgB.var;    
    
    msg = a.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_A);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_A);

   
    
    

