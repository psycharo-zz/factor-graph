function exampleUnivariateGaussian
%EXAMPLEUNIVARIATEGAUSSIAN test framework on a simple univariate gaussian
% saves resulting figures to 'figures'


MU = -3;
SD = 2;
NUM_POINTS = 150;
MAX_NUM_ITERS = 10;
POINTS = normrnd(MU, SD, 1, NUM_POINTS);


[estMU, estPREC, iters, lbEvidence] = mexfactorgraph('examples', 'univariate', POINTS, MAX_NUM_ITERS);

X = -6*SD:1e-2:6*SD;

h = figure;
hold on;
histDistr(POINTS, 50);
plot(X, pdf('normal', X, MU, SD), '-r', 'LineWidth', 2);
plot(X, pdf('normal', X, estMU, sqrt(1.0/estPREC)), '--b', 'LineWidth', 2);


legend('Data histogram', 'true distribution', 'VMP estimation', 'Location', 'NorthEast');


fprintf('after %d iterations, LB = %f\n', iters, lbEvidence);
print(h, 'figures/val-univariate');

% TODO: hyperparameters, plot data histogram
end

