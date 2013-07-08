function plotGMM(X, means, covs, weights)
%PLOTGMM plot a mixture of gaussians: only 1D and 2D are supported
DIMS = size(X, 2);
if DIMS == 1
    rcovs(1,1,1:length(weights)) = covs;
    plot(X, pdf(gmdistribution(means', rcovs, weights), X'), '-r', 'LineWidth', 3);
elseif DIMS == 2
    plot(X(:,1),X(:,2),'.k'); %Plot data
    hold on;
    numMixtures = length(weights);
    % Plot the gaussians
    hold on;
    for m = 1:numMixtures
        if weights(m) > 1e-2
            h = plotGaussian(means(m,:), covs(:,:,m), 2);
            set(h, 'LineWidth', 2);
            set(h, 'color', 'm');
        end
    end
else
    error('unsupported');
end
        
return

