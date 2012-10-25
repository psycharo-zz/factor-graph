function test_suite = testEvidenceNode
initTestSuite;

function testScalar
node = EvidenceNode;
dest = EvidenceNode;
node.setDest(dest);
dest.setDest(node);
msg = struct('type',1,'mean',2, 'var',3);
node.setInitial(msg);
assertEqual(msg.mean, dest.evidence().mean);
assertEqual(msg.var, dest.evidence().var);
    
    



