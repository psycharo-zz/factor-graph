function exampleImageSegmentation(vb)
%IMAGESEGMENTATIONEXAMPLE image segmentation take an image and do color segmentation

FILES = {'image0.jpg', 'image1.jpg', 'image2.jpg'};

fname = 'image1.jpg';
numMixtures = 8;

imgRgb = double(imread(fname));
[X,cols,rows] = getImageData(imgRgb);
if vb == 1
    [means, covs, weights, evidence, iters, resp] = trainGMM_VB(X, numMixtures);
    outname = strcat('vb-', int2str(numMixtures), fname);
else
    distr = gmdistribution.fit(X, numMixtures);
    means = distr.mu;
    [idx,nlogl,resp] = cluster(distr,X);
    resp = resp';
    outname = strcat('em-', int2str(numMixtures), fname);
end

imgSegm = getSegmentedImage(resp, means, rows, cols);
imshow(imgSegm)
imwrite(imgSegm, fullfile('figures', 'segmented', outname));
PSNR = psnrRGB(imgRgb, imgSegm)

return


function value = psnrRGB(A, B)

% Read the dimensions of the image.
[rows, cols, ~] = size(A);

% Calculate mean square error of R, G, B.   
mseRImage = (double(A(:,:,1)) - double(B(:,:,1))) .^ 2;
mseGImage = (double(A(:,:,2)) - double(B(:,:,2))) .^ 2;
mseBImage = (double(A(:,:,3)) - double(B(:,:,3))) .^ 2;

mseR = sum(sum(mseRImage)) / (rows * cols);
mseG = sum(sum(mseGImage)) / (rows * cols);
mseB = sum(sum(mseBImage)) / (rows * cols);

% Average mean square error of R, G, B.
mse = (mseR + mseG + mseB)/3;

% Calculate PSNR (Peak Signal to noise ratio).
value = 10 * log10( 255^2 / mse);

return




