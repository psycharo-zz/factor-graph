classdef even_parity_node < factor_node
%EVEN_PARITY is a n-port event parity check node
%   It is a bivalue node such that the message are row vector of 2 elements.
%   The mod 2 sum of the input message are zero.
%
% Description of factor_fun
%   input: in_msg, from_id, to_id
%   output: msg(0) = in_msg{1}(1)*in_msg{2}(1) + in_msg{1}(2)*in_msg{2}(2);
%   for one in_msg, msg = in_msg{1}
%   for more than one in_msg, firt set msg = in_msg{1}, apply the following
%   operation recursively for the rest of the messages:
%                msg = [...
%                    msg(1)*in_msg{i}(1) + msg(2)*in_msg{i}(2),...
%                    msg(1)*in_msg{i}(2) + msg(2)*in_msg{i}(1)];

   properties
   end

   methods
       function s = even_parity_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
           msg = in_msg{1};
           for i = 2:size(in_msg,1)
               msg = [...
                   msg(1)*in_msg{i}(1) + msg(2)*in_msg{i}(2),...
                   msg(1)*in_msg{i}(2) + msg(2)*in_msg{i}(1)];
           end
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end
   end
end
