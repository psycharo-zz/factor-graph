function test_suite = testEqualityNode
%TESTADDNODE Summary of this function goes here
%   Detailed explanation goes here
initTestSuite;

function testScalarGaussian
    node = EqualityNode;

    a = EvidenceNode;
    b = EvidenceNode;
    c = EvidenceNode;

    nwk = Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);

    a.setInitial(struct('type',1, 'mean',10, 'var',5));
    b.setInitial(struct('type',1, 'mean',20, 'var',4));
    msg = c.evidence();
    

    EXPECTED_MEAN = 15.555555555555557;
    EXPECTED_VAR = 2.2222222222222223;

    
    assertEqual(size(msg.mean,2), 1);
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);



function testMultivariateGaussian

    load('data_gaussian.mat');

    node = EqualityNode;

    a = EvidenceNode;
    b = EvidenceNode;
    c = EvidenceNode;

    nwk = Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(c, node);

    a.setInitial(struct('type',1, 'mean',mean1, 'var',var1));
    b.setInitial(struct('type',1, 'mean',mean2, 'var',var2));
    msg = c.evidence();

    EXPECTED_VAR=var3;
    EXPECTED_MEAN=mean3';

    assertEqual(size(msg.mean), size(EXPECTED_MEAN));
    assertEqual(size(msg.var), size(EXPECTED_VAR));
    
    assertElementsAlmostEqual(msg.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(msg.var, EXPECTED_VAR);





