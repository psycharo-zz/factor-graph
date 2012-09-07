classdef noisy_or_node < cp_node
%NOISY_OR is a subclass of cp
%   The noisy or noise has multiple parent and a child port.  Whenever one
%   of the parent is true, the child will be true.
%   It is a bivalue node such that the messages have two entries only.
%   The cpt of the noisy or node is in the form of row vector (where as the
%   cpt of another cp nodes are n-dimension matrix).  Each of element in
%   the cpt specifies the probability of the child when the particular 
%   parent is true.
%   e.g.   P(r|p) P(r|q)
%   cpt = [0.800 0.700];  cpt(1) & cpt(2) corresponding to parent p & q
%   To form a leaky noisy or node, just feed one of the parent connection 
%   with a constant message [0 1] and put a small conditional probability
%   in the corresponding position in the cpt.
%   The msg to the parent and to the child are calculated in different
%   ways:
%   for message to child
%   let PI = product of (1 - cpt for in_msg{j})*in_msg{j}(2) for all j
%   msg(1) = PI
%   msg(2) = 1 - msg(1)
%   for message to a parent correponding to index i
%   let PI(i) = product of (1 - cpt for in_msg{j})*in_msg{j}(2) for all j
%   ~= i
%   assume [c0, c1] is message from child
%   msg(1) = c1 - (c1 - c0)*PI(i)
%   msg(2) = c1 - cpt(i) *(c1 - c0)*PI(i)
%   Ref. to the Probabilistic Reasoning in Intelligent Systems: Networks of
%   Plausible Inference by Judea Pearl.


   properties
   end

   methods
       function s = noisy_or_node(id)
           s = s@cp_node(id);
       end
       
       function error_check(s)
           error_check@cp_node(s);
           if size(s.cpt,1) > 1
               sprintf('Node %0.0f ERROR from error_check: cpt is not a row vector !!!\n',s.id)
           end
       end

       function msg = factor_fun(s, in_msg, from_id, to_id)
           % index for summarization
           sum_var = find(s.link_id == to_id);
           num_link = size(s.linklist,2);
           tmp_msg = 1;
           if sum_var == num_link
               % message to child
               for i = 1:num_link-1
                   % from product from message from all parents
                   var = find(s.link_id == from_id(i));
                   tmp_msg = tmp_msg*(1-s.cpt(var)*in_msg{i}(2));
               end
               msg = [tmp_msg, 1-tmp_msg];
           else
               % message to parent
               for i = 1:num_link-1
                   if s.link_id(num_link) == from_id(i)
                       % message from child
                       lamda = in_msg{i};
                   else
                       % message from other parent
                       var = find(s.link_id == from_id(i));
                       tmp_msg = tmp_msg*(1-s.cpt(var)*in_msg{i}(2));
                   end
               end
               var = find(s.link_id == to_id);
               msg = [lamda(2) - (lamda(2) - lamda(1))*tmp_msg,...
                      lamda(2) - s.cpt(var)*(lamda(2) - lamda(1))*tmp_msg];
           end % if sum_var...
           sum_msg = sum(msg);
           if sum_msg == 0
               msg = ones(size(msg));
           end
           msg = msg/sum(msg);
       end % function msg = factor_fun
   end % methods
end 
