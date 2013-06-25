function result = repeatSignal(signalB, len)
% ADDNOISE adding n
%   Input:
%       |signalA| - 1st signal in time domain
%       |signalB|  - 2nd signal in time domain
%       |snr|    - signalA to signalB ratio
    % TODO: ugly
    if length(signalB) < len
        factor = idivide(int32(len), int32(length(signalB)), 'floor');
        residual = len - length(signalB) * factor;
        result = vertcat(repmat(signalB, factor, 1), signalB(1:residual));
    else
        result = signalB(1:len);
    end
return