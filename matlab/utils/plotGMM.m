function plotGMM(X, means, precs, weights)
%PLOTGMM plot a mixture of gaussians 

covs = zeros();
covs(1,1,1:length(weights)) = 1 ./ precs;
distr = gmdistribution(means', covs, weights);
plot(X, pdf(distr, X'), '-r', 'LineWidth', 3);
end

