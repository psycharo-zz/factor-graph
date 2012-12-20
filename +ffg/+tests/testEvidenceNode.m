function test_suite = testEvidenceNode
initTestSuite;

function testScalar
% the simplest test ever
nwk = ffg.Network;
node = ffg.EvidenceNode(nwk);
dest = ffg.EvidenceNode(nwk);
nwk.addEdge(node, dest);

% input
msg = ffg.messages.gaussVariance(2, 3);
%

node.propagate(msg);
assertEqual(msg.mean, dest.evidence().mean);
assertEqual(msg.var, dest.evidence().var);
    
    



