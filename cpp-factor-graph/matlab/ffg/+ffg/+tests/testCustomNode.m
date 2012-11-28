function test_suite = testCustomNode
%TESTCUSTOMNODE Summary of this function goes here
%   Detailed explanation goes here
initTestSuite;

function testAddForwardNode
    nwk = ffg.Network;
    node = ffg.CustomNode;
        
    node.setFunction('ffg.tests.customnode_function_gauss');
    
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    EXPECTED_MEAN_C = 30;
    EXPECTED_VAR_C = 9;
    
    a.propagate(struct('type', 'VARIANCE','mean',10,'var',5));
    b.propagate(struct('type', 'VARIANCE','mean',20,'var',4));

    msg = c.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_C);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_C);
    
    
function testAddBackwardNode
    nwk = ffg.Network;
    node = ffg.CustomNode;
    
    node.setFunction('ffg.tests.customnode_function_gauss');
    
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);


    c.propagate(struct('type', 'VARIANCE', 'mean',10, 'var',5));
    b.propagate(struct('type', 'VARIANCE', 'mean',20, 'var',4));

    EXPECTED_MEAN_A = -10;
    EXPECTED_VAR_A = 9;
    msg = a.evidence();
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN_A);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR_A);


    
    

