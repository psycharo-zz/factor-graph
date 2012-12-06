function test_suite = testEvidenceNode
initTestSuite;

function testScalar
% the simplest test ever
nwk = ffg.Network;
node = ffg.EvidenceNode;
dest = ffg.EvidenceNode;
nwk.addEdge(node, dest);

% input
msg = ffg.gaussMessage(2, 3, 'VARIANCE');
%

node.propagate(msg);
assertEqual(msg.mean, dest.evidence().mean);
assertEqual(msg.var, dest.evidence().var);
    
    



