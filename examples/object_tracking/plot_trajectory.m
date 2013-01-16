%   Copyright 2010 The MathWorks, Inc.
function plot_trajectory(z,y,y_p)
%#codegen
eml.extrinsic('title','xlabel','ylabel','plot','axis','pause');
title('Trajectory of object [blue] its Kalman estimate [green, red]');
xlabel('horizontal position');
ylabel('vertical position');
plot(z(1), z(2), 'bx-');
plot(y(1), y(2), 'go-');

if nargin == 3
    plot(y_p(1), y_p(2), 'r*-');
end

axis([-1.1, 1.1, -1.1, 1.1]);

pause(0.02);
end            % of the function