function plotGMM(X, means, covs, weights)
%PLOTGMM plot a mixture of gaussians 
rcovs(1,1,1:length(weights)) = covs;
plot(X, pdf(gmdistribution(means', rcovs, weights), X'), '-r', 'LineWidth', 3);
end

