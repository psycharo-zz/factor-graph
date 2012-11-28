function test_suite = testEstimateMultiplicationNode
initTestSuite;

function testForward

load('+ffg/+tests/data_gaussian');

nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;

MATRIX = TestMultiplication.INPUT1.matrix;
A = ffg.MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = struct('type', 'VARIANCE', 'mean', TestMultiplication.INPUT1.mean, 'var', TestMultiplication.INPUT1.var);

x.propagate(INPUT_MSG);

RESULT_MSG = y.evidence();

MEAN_EXPECTED = MATRIX * TestMultiplication.INPUT1.mean';
VAR_EXPECTED = MATRIX * TestMultiplication.INPUT1.var * MATRIX';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);

function testBackward

load('+ffg/+tests/data_gaussian');

nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;

MATRIX = TestMultiplication.INPUT1.matrix;
A = ffg.MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = struct('type', 'VARIANCE', 'mean', TestMultiplication.INPUT1.mean, 'var', TestMultiplication.INPUT1.var);

y.propagate(INPUT_MSG);

RESULT_MSG = x.evidence();

VAR_EXPECTED = inv(MATRIX' * inv(TestMultiplication.INPUT1.var) * MATRIX);
MEAN_EXPECTED = VAR_EXPECTED * MATRIX' * inv(TestMultiplication.INPUT1.var) * TestMultiplication.INPUT1.mean';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);



function testEstimateBackward

load('+ffg/+tests/data_gaussian');

nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;
estmt = ffg.EvidenceNode;
A = ffg.EstimateMultiplicationNode;


nwk.addEdge(x, A);
nwk.addEdge(A, y);
nwk.addEdgeTagged(A, estmt, 'estimate', '');



INPUT_MSG_X = struct('type', 'VARIANCE', 'mean', TestMultiplication.INPUT1.mean, 'var', TestMultiplication.INPUT1.var);
MSG_SIZE = length(INPUT_MSG_X.mean);
INPUT_MSG_Y = struct('type', 'VARIANCE', 'mean', eye(1, MSG_SIZE), 'var', eye(MSG_SIZE, MSG_SIZE));

A.setMatrix(eye(MSG_SIZE, MSG_SIZE));

x.propagate(INPUT_MSG_X);
y.propagate(INPUT_MSG_Y);


RESULT_MSG = estmt.evidence();

%  m_a = m_y[1] * m_x / ||m_x||^2
MEAN_EXPECTED = INPUT_MSG_Y.mean(1) * INPUT_MSG_X.mean / sum(INPUT_MSG_X.mean.^2);
%  W_a = m_x * m_x^T / sd_y2
PRECISION_EXPECTED = INPUT_MSG_X.mean' * INPUT_MSG_X.mean / INPUT_MSG_Y.var(1);

assertElementsAlmostEqual(RESULT_MSG.mean, MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.precision, PRECISION_EXPECTED);










    
    
