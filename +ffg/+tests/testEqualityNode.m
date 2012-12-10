function test_suite = testEqualityNode
%TESTADDNODE Summary of this function goes here
%   Detailed explanation goes here
initTestSuite;

function testScalarGaussian
    % simle 

    node = ffg.EqualityNode;

    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);

    msgA = ffg.gaussMessage(10, 5, 'VARIANCE');
    msgB = ffg.gaussMessage(20, 4, 'VARIANCE');
    
    a.propagate(msgA);
    b.propagate(msgB);

    msg = c.evidence();
    
    
    PRECISION_C = (1 / msgA.var + 1 / msgB.var);

    EXPECTED_MEAN = 1 / PRECISION_C * (1 / msgA.var * msgA.mean + 1 / msgB.var * msgB.mean);
    EXPECTED_VAR = 1 / PRECISION_C;

    
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);

    return;

function testMultivariateGaussian
% multidimensional gaussians
   
    DIMENSIONALITY = 4;

    node = ffg.EqualityNode;
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);
    
    % randomly generating
    msgA = ffg.gaussMessage(randn(1, DIMENSIONALITY), randn(DIMENSIONALITY, DIMENSIONALITY), 'VARIANCE');
    msgB = ffg.gaussMessage(randn(1, DIMENSIONALITY), randn(DIMENSIONALITY, DIMENSIONALITY), 'VARIANCE');
    
    a.propagate(msgA);
    b.propagate(msgB);
    msg = c.evidence();
   

    PRECISION = inv(msgA.var) + inv(msgB.var);
    EXPECTED_VAR=inv(PRECISION);
    EXPECTED_MEAN=(inv(PRECISION) * (inv(msgA.var) * msgA.mean' + inv(msgB.var) * msgB.mean'))';

    assertEqual(size(msg.mean), size(EXPECTED_MEAN));
    assertEqual(size(msg.var), size(EXPECTED_VAR));
    
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);

    return;
    
function testNoInformation
    % testing whether the 0 precision will cause no problems

    node = ffg.EqualityNode;
    node.setType('PRECISION');
    
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);
    
    % number of dimensions
    DIM = 3;
    
    msgA = ffg.gaussMessage([1,2,3], eye(DIM, DIM), 'PRECISION');
    msgB = ffg.gaussMessage(zeros(1,3), zeros(DIM, DIM), 'PRECISION');
    a.propagate(msgA);
    b.propagate(msgB);
    
    EXPECTED_MESSAGE = msgA;
    
    assertElementsAlmostEqual(EXPECTED_MESSAGE.mean, c.evidence().mean);
    assertElementsAlmostEqual(EXPECTED_MESSAGE.precision, c.evidence().precision);
    





