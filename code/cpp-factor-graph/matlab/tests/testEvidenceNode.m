function test_suite = testEvidenceNode
initTestSuite;

function testScalar

nwk = Network;
node = EvidenceNode;
dest = EvidenceNode;
nwk.addEdge(node, dest);

msg = struct('type',1,'mean',2, 'var',3);
node.propagate(msg);
assertEqual(msg.mean, dest.evidence().mean);
assertEqual(msg.var, dest.evidence().var);
    
    



