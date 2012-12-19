function test_suite = testNetwork
%TESTNETWORK testing ffg.Network
    initTestSuite;
    
    
function testAdjacencyMatrix4Nodes
% simple 4-node network
    a = ffg.EvidenceNode;
    b = ffg.EvidenceNode;
    c = ffg.EvidenceNode;
    node = ffg.AddNode;

    nwk = ffg.Network;
    nwk.addEdge(a, node);
    nwk.addEdge(b, node);
    nwk.addEdge(node, c);
    
    EXPECTED_MATRIX = zeros(4, 4);
    EXPECTED_MATRIX(1, 4) = 1;
    EXPECTED_MATRIX(2, 4) = 1;
    EXPECTED_MATRIX(4, 3) = 1;
    
    
    assertEqual(nwk.adjacencyMatrix(), EXPECTED_MATRIX);
    
    
function testAdjacencyMatrixKalmanFilter
% larger network (kalman filter)
    xin = ffg.EvidenceNode; %1
    xout = ffg.EvidenceNode;%2
    n = ffg.EvidenceNode;%3
    y = ffg.EvidenceNode;%4
    e = ffg.EqualityNode;%5
    a = ffg.AddNode;%6
    u = ffg.EvidenceNode;%7
    b = ffg.AddNode;%8
    
    nwk = ffg.Network;
   
    nwk.addEdge(xin, e);
    nwk.addEdge(e, b);
    nwk.addEdge(u, b);
    nwk.addEdge(b, xout);
    nwk.addEdge(e, a);
    nwk.addEdge(a, y);
    nwk.addEdge(n, a);

    
    EXPECTED_MATRIX = zeros(8, 8);
    EXPECTED_MATRIX(1, 5) = 1;
    EXPECTED_MATRIX(5, 8) = 1;
    EXPECTED_MATRIX(7, 8) = 1;
    EXPECTED_MATRIX(8, 2) = 1;
    EXPECTED_MATRIX(5, 6) = 1;
    EXPECTED_MATRIX(6, 4) = 1;
    EXPECTED_MATRIX(3, 6) = 1;
    
    assertEqual(nwk.adjacencyMatrix(), EXPECTED_MATRIX);


function testAdjacencyMatrixMult
% test for multiplication node
    nwk = ffg.Network;
    
    
    xin = ffg.EvidenceNode;
    mult = ffg.MultiplicationNode;
    xout = ffg.EvidenceNode;
    
    nwk.addEdge(xin, mult);
    nwk.addEdge(mult, xout);
    
    EXPECTED_MATRIX = zeros(3,3);
    
    EXPECTED_MATRIX(1,2) = 1;
    EXPECTED_MATRIX(2,3) = 1;
    
    assertEqual(nwk.adjacencyMatrix(), EXPECTED_MATRIX);
    
    


