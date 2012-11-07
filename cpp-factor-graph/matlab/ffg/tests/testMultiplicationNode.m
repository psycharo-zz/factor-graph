function test_suite = testMultiplicationNode
initTestSuite;

function testForward

load('data_gaussian');

nwk = Network;
x = EvidenceNode;
y = EvidenceNode;

MATRIX = TestMultiplication.INPUT1.matrix;
A = MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = struct('type', 1, 'mean', TestMultiplication.INPUT1.mean, 'var', TestMultiplication.INPUT1.var);

x.propagate(INPUT_MSG);

RESULT_MSG = y.evidence();

MEAN_EXPECTED = MATRIX * TestMultiplication.INPUT1.mean';
VAR_EXPECTED = MATRIX * TestMultiplication.INPUT1.var * MATRIX';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);

function testBackward

load('data_gaussian');

nwk = Network;
x = EvidenceNode;
y = EvidenceNode;

MATRIX = TestMultiplication.INPUT1.matrix;
A = MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = struct('type', 1, 'mean', TestMultiplication.INPUT1.mean, 'var', TestMultiplication.INPUT1.var);

y.propagate(INPUT_MSG);

RESULT_MSG = x.evidence();

VAR_EXPECTED = inv(MATRIX' * inv(TestMultiplication.INPUT1.var) * MATRIX);
MEAN_EXPECTED = VAR_EXPECTED * MATRIX' * inv(TestMultiplication.INPUT1.var) * TestMultiplication.INPUT1.mean';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);




    
    