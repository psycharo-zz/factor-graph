function test_suite = testEquMultNode
initTestSuite;

function testForward

    nwk = ffg.Network;
    x = ffg.EvidenceNode;
    y = ffg.EvidenceNode;
    z = ffg.EvidenceNode;

    % input
    MATRIX = eye(1, 3);
    INPUT_X = ffg.gaussMessage([1, 1, 1], eye(3,3), 'VARIANCE');
    INPUT_Y = ffg.gaussMessage(0.5, 0, 'VARIANCE');
    %

    equMult = ffg.EquMultNode;
    equMult.setMatrix(MATRIX);

    % note that the direction is important here
    nwk.addEdge(x, equMult);
    nwk.addEdge(z, equMult);
    nwk.addEdge(equMult, y);

    y.propagate(INPUT_Y);
    x.propagate(INPUT_X);


    RESULT = z.evidence();

    % see (Table 4.1, Korl 2005)
    A = MATRIX;
    mean_x = INPUT_X.mean';
    var_x = INPUT_X.var;
    var_y = INPUT_Y.var;
    mean_y = INPUT_Y.mean';
    G = inv(var_y + A * var_x * A');
    EXPECTED_MEAN = (mean_x + var_x * A' * G * (mean_y - A * mean_x))';
    EXPECTED_VAR = var_x - var_x * A' * G * A * var_x;


    assertElementsAlmostEqual(RESULT.mean, EXPECTED_MEAN);
    assertElementsAlmostEqual(RESULT.var, EXPECTED_VAR);



















