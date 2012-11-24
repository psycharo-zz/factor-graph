function test_suite = testEqualityNode
%TESTADDNODE Summary of this function goes here
%   Detailed explanation goes here
initTestSuite;

function testScalarGaussian
    node = ffg.EqualityNode;

    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);

    a.propagate(struct('type','VARIANCE', 'mean',10, 'var',5));
    b.propagate(struct('type','VARIANCE', 'mean',20, 'var',4));
    msg = c.evidence();
    

    EXPECTED_MEAN = 15.555555555555557;
    EXPECTED_VAR = 2.2222222222222223;

    
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);

    return;

function testMultivariateGaussian

    load('data_gaussian.mat');

    node = ffg.EqualityNode;
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);

    a.propagate(ffg.gaussMessage(TESTmean1, TESTvar1, 'VARIANCE'));
    b.propagate(ffg.gaussMessage(TESTmean2, TESTvar2, 'VARIANCE'));
    msg = c.evidence();

    EXPECTED_VAR=TESTvar3;
    EXPECTED_MEAN=TESTmean3';

    assertEqual(size(msg.mean), size(EXPECTED_MEAN));
    assertEqual(size(msg.var), size(EXPECTED_VAR));
    
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);

    return;
    
function testNoInformation

    node = ffg.EqualityNode;
    node.setType('PRECISION');
    
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);
    
    M = 3;
    
    a.propagate(ffg.gaussMessage([1,2,3], eye(M,M), 'PRECISION'));
    b.propagate(ffg.gaussMessage(zeros(1,3), zeros(M,M), 'PRECISION'));
    
    EXPECTED_MESSAGE = ffg.gaussMessage([1,2,3], eye(M,M), 'PRECISION');
    assertElementsAlmostEqual(EXPECTED_MESSAGE.mean, c.evidence().mean);
    assertElementsAlmostEqual(EXPECTED_MESSAGE.precision, c.evidence().precision);
    





