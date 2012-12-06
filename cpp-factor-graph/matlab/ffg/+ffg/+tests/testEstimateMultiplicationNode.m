function test_suite = testEstimateMultiplicationNode
initTestSuite;

function testForward


DIM = 3;

nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;


MATRIX = randn(DIM, DIM);

A = ffg.MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = ffg.gaussMessage(randn(1, DIM), randn(DIM, DIM), 'VARIANCE');

x.propagate(INPUT_MSG);

RESULT_MSG = y.evidence();


MEAN_EXPECTED = MATRIX * INPUT_MSG.mean';
VAR_EXPECTED = MATRIX * INPUT_MSG.var * MATRIX';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);

function testBackward

DIM = 3;


nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;

MATRIX = randn(DIM, DIM);
A = ffg.MultiplicationNode;
A.setMatrix(MATRIX);

nwk.addEdge(x, A);
nwk.addEdge(A, y);

% msg = 
INPUT_MSG = ffg.gaussMessage(randn(1, DIM), randn(DIM, DIM), 'VARIANCE');

% one propagation iteration
y.propagate(INPUT_MSG);
RESULT_MSG = x.evidence();

VAR_EXPECTED = inv(MATRIX' * inv(INPUT_MSG.var) * MATRIX);
MEAN_EXPECTED = VAR_EXPECTED * MATRIX' * inv(INPUT_MSG.var) * INPUT_MSG.mean';

assertElementsAlmostEqual(RESULT_MSG.mean', MEAN_EXPECTED);
assertElementsAlmostEqual(RESULT_MSG.var, VAR_EXPECTED);



function testEstimateBackward


nwk = ffg.Network;
x = ffg.EvidenceNode;
y = ffg.EvidenceNode;
estmt = ffg.EvidenceNode;
A = ffg.EstimateMultiplicationNode;


nwk.addEdge(x, A);
nwk.addEdge(A, y);
nwk.addEdgeTagged(A, estmt, 'estimate', '');


DIM = 4;

INPUT_MSG_X = ffg.gaussMessage(randn(1, DIM), randn(DIM, DIM), 'VARIANCE');
MSG_SIZE = length(INPUT_MSG_X.mean);
INPUT_MSG_Y = ffg.gaussMessage(eye(1, DIM), eye(DIM, DIM), 'VARIANCE');

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










    
    
