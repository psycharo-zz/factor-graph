function test_suite = testEvidenceNode
initTestSuite;

function testScalar

nwk = ffg.Network;
node = ffg.EvidenceNode;
dest = ffg.EvidenceNode;
nwk.addEdge(node, dest);

msg = struct('type','VARIANCE','mean',2, 'var',3);
node.propagate(msg);
assertEqual(msg.mean, dest.evidence().mean);
assertEqual(msg.var, dest.evidence().var);
    
    



