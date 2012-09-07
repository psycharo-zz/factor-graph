classdef factor_node < handle
% FACTOR_NODE class implements the behaviour of factor nodes in a factor
% graph.  It is derived from handle class so that a factor node object can 
% be referenced by another object.
% 
% Descritpion of methods:
%     FACTOR_NODE instantiate an factor_node object.
%     RESET reset the factor_node object but keeps the linklist.
%     SETUP_LINK initialize the linklist and link_id.
%     RX_MSG performs the sum-product algorithm.
%     FACTOR_FUN, an abstract function, must be defined in derived classes.
%
% Usage:
% Initialize a factor nodes m,n,o with id = 1,2,3 respectively
%     m = factor_node(1);
%     n = factor_node(2);
%     o = factor_node(3);
% Set up the linklist such that m,n,o are linked
%     m.setup_link({n});
%     n.setup_link({m o});
%     o.setup_link({n});
% Reset nodes
%     m.reset();
%     n.reset();
%     o.reset();

   properties
       id             % id of the object
       linklist       % linklist, 1xn cell array
       link_id        % link_id, 1xn array
       state          % state of the node
       inbound_msg    % inbound message, nx1 cell array
       from_node      % from node, 1xn cell array
       from_id        % from id, 1xn array
       outbound_msg   % outbound message nx1 cell array
       to_node        % to node, nx1 cell array
       to_id          % to id, 1xn array
       missing_node   % missing node, 1xn cell array
   end
   
   properties (Constant)
       % define possible state
       st_idle = 0;
       st_active = 1;
       st_complete = 2;
   end
   
   methods
       function FN = factor_node(id)
           % constructor
           FN.id = id;
           FN.linklist = {};
           FN.state = FN.st_idle;
           FN.inbound_msg = {};
           FN.outbound_msg = {};
           FN.from_node = {};
           FN.to_node = {};
           FN.missing_node = {};
           FN.from_id = [];
           FN.to_id = [];
       end
       
       function reset(FN)
           % reset properties
           FN.state = FN.st_idle;
           FN.inbound_msg = {};
           FN.outbound_msg = {};
           FN.from_node = {};
           FN.to_node = {};
           FN.missing_node = FN.linklist;
           FN.from_id = [];
           FN.to_id = [];
       end
       
       function setup_link(FN,linklist)
           global loopy
           % update linklist
           if (iscell(linklist) ~= 1) || (size(linklist,1) ~= 1)
               sprintf('Node %0.0f ERROR from setup_link: linklist must be 1xn cell array !!!\n',FN.id)
           else
               FN.linklist = linklist;
               FN.missing_node = linklist;
               FN.link_id = [];
               for i = 1:size(linklist,2)
                   FN.link_id = [FN.link_id linklist{i}.id];
               end
               if loopy == 1
                   FN.inbound_msg = {};
                   for i = 1:size(FN.linklist,2)
                       FN.inbound_msg{i,1} = [];
                       FN.outbound_msg{i,1} = [];
                   end
                   FN.to_node = FN.linklist;
                   FN.from_node = FN.linklist;
                   FN.to_id = FN.link_id;
                   FN.from_id = FN.link_id;
               end
           end
       end
       
       function append_link(FN,linklist)
           global loopy
           if (iscell(linklist) ~= 1) || (size(linklist,1) ~= 1)
               sprintf('Node %0.0f ERROR from append_link: linklist must be 1xn cell array !!!\n',FN.id)
           else
               FN.linklist = [FN.linklist linklist];
               FN.missing_node = FN.linklist;
               for i = 1:size(linklist,2)
                   FN.link_id = [FN.link_id linklist{i}.id];
               end
               if loopy == 1
                   FN.inbound_msg = {};
                   for i = 1:size(FN.linklist,2)
                       FN.inbound_msg{i,1} = [];
                       FN.outbound_msg{i,1} = [];
                   end
                   FN.to_node = FN.linklist;
                   FN.from_node = FN.linklist;
                   FN.to_id = FN.link_id;
                   FN.from_id = FN.link_id;
               end
           end
       end
       
       function prepend_link(FN,linklist)
           global loopy
           if (iscell(linklist) ~= 1) || (size(linklist,1) ~= 1)
               sprintf('Node %0.0f ERROR from prepend_link: linklist must be 1xn cell array !!!\n',FN.id)
           else
               FN.linklist = [linklist FN.linklist];
               FN.missing_node = FN.linklist;
               for i = 1:size(linklist,2)
                   FN.link_id = [linklist{i}.id FN.link_id];
               end
               if loopy == 1
                   FN.inbound_msg = {};
                   for i = 1:size(FN.linklist,2)
                       FN.inbound_msg{i,1} = [];
                       FN.outbound_msg{i,1} = [];
                   end
                   FN.to_node = FN.linklist;
                   FN.from_node = FN.linklist;
                   FN.to_id = FN.link_id;
                   FN.from_id = FN.link_id;
               end
           end
       end
       
       function error_check(FN)
           if isempty(FN.id)
               sprintf('Node %0.0f ERROR from error_check: no node id !!!\n',FN.id)
           elseif isempty(FN.linklist)
               sprintf('Node %0.0f ERROR from error_check: empty linklist !!!\n',FN.id)
           end
       end
       
       function update_node(FN, default_msg)
           global loopy linear_scalar
           if loopy == 1
               if linear_scalar == 1
                   default_msg = [0 1e99];
               end
               for i = 1:size(FN.linklist,2)
                   % i is the destination of message
                   % collect all message except from i
                   in_msg = {}; fm_id = [];
                   for j = 1:size(FN.linklist,2)
                       if (i == j)
                           continue;
                       end
                       if isempty(FN.inbound_msg{j})
                           FN.inbound_msg{j} = default_msg;
                       end
                       in_msg{size(in_msg,1)+1,1} = FN.inbound_msg{j};
                       fm_id = [fm_id; FN.link_id(j)];
                   end % for j = ...
                   % execute factor function
                   snd_msg = FN.factor_fun(in_msg, fm_id, FN.link_id(i));
                   % send message
                   FN.linklist{i}.rx_msg(FN, snd_msg);
                   % update outbound record
                   %k = find(FN.to_id == FN.from_id(i));
                   %FN.to_node{k} = FN.from_node{i};
                   FN.outbound_msg{i} = snd_msg;
               end % for i = ...
           else
               sprintf('Node %0.0f ERROR from update_node: update_node is not available for non-loopy network !!!\n',FN.id)
           end % if loopy ...
       end %function update_node
   end
   
   methods (Access = protected)
       function rx_msg(FN, from_node, msg)
           global loopy linear_scalar
           if loopy == 1
               if linear_scalar == 1
                   tmp_msg = [0 1e99];
               else
                   tmp_msg = ones(size(msg));
               end
               for i = 1:size(FN.linklist,2)
                   if eq(FN.linklist{i},from_node)
                       FN.inbound_msg{i} = msg;
                   elseif isempty(FN.inbound_msg{i})
                       FN.inbound_msg{i} = tmp_msg;
                   end
               end
               %FN.from_node{1,size(FN.from_node,2)+1} = from_node;
               %FN.inbound_msg{size(FN.inbound_msg,1)+1,1} = msg;
               %FN.from_id = [FN.from_id from_node.id];
           else
               switch FN.state
                   case FN.st_idle
                       FN.from_node{1,size(FN.from_node,2)+1} = from_node;
                       FN.inbound_msg{size(FN.inbound_msg,1)+1,1} = msg;
                       FN.from_id = [FN.from_id from_node.id];
                       % find missing node by elimination
                       tmp = {};
                       for i = 1:size(FN.missing_node,2)
                           if eq(from_node,FN.missing_node{i})
                               % from_node == missing_node{i}, exclude node
                               continue
                           else
                               tmp{1,size(tmp,2)+1} = FN.missing_node{i};
                           end
                       end
                       % update missing node
                       FN.missing_node = tmp;
                       if size(FN.missing_node,2) == 1
                           % pending one last message
                           FN.state = FN.st_active;
                           % construct message
                           snd_msg = FN.factor_fun(FN.inbound_msg,FN.from_id,FN.missing_node{1}.id);
                           % send message to missing node
                           FN.missing_node{1}.rx_msg(FN, snd_msg);
                           % update outbound record
                           %FN.to_node{size(FN.to_node,1)+2,1} = FN.missing_node{1};
                           FN.to_node = FN.missing_node;
                           FN.outbound_msg{size(FN.outbound_msg,1)+1,1} = snd_msg;
                           FN.to_id = [FN.to_id FN.missing_node{1}.id];
                       end
                   case FN.st_active
                       if eq(from_node,FN.missing_node{1})
                           FN.from_node{1,size(FN.from_node,2)+1} = from_node;
                           FN.inbound_msg{size(FN.inbound_msg,1)+1,1} = msg;
                           FN.from_id = [FN.from_id from_node.id];
                           % all message received
                           FN.state = FN.st_complete;
                           for i = 1:size(FN.from_node,2)
                               % skip missing node
                               if eq(FN.from_node{i},FN.missing_node{1})
                                   continue
                               end
                               in_msg = {}; fm_id = [];
                               % i is the destination of message
                               % collect all message except from i
                               for j = 1:size(FN.from_node,2)
                                   if i == j
                                       continue
                                   end
                                   in_msg{size(in_msg,1)+1,1} = FN.inbound_msg{j};
                                   fm_id = [fm_id; FN.from_id(j)];
                               end % for j = ...
                               % execute factor function
                               snd_msg = FN.factor_fun(in_msg, fm_id, FN.from_id(i));
                               % send message
                               FN.from_node{i}.rx_msg(FN, snd_msg);
                               % update outbound record
                               %FN.to_node{size(FN.to_node,1)+2,1} = FN.from_node{i};
                               FN.to_node{1,size(FN.to_node,2)+1} = FN.from_node{i};
                               FN.outbound_msg{size(FN.outbound_msg,1)+1,1} = snd_msg;
                               FN.to_id = [FN.to_id FN.from_node{i}.id];
                           end % for i = ...
                       end % if (size(FN.linklist...
                   case FN.st_complete
                       % handle update message
                       i = find(FN.from_id == from_node.id);
                       FN.from_node{i} = from_node;
                       FN.from_id(i) = from_node.id;
                       FN.inbound_msg{i} = msg;
                       for i = 1:size(FN.from_node,2)
                           % skip missing node
                           if eq(FN.from_node{i},from_node)
                               continue
                           end
                           in_msg = {}; fm_id = [];
                           % i is the destination of message
                           % collect all message except from i
                           for j = 1:size(FN.from_node,2)
                               if i == j
                                   continue
                               end
                               in_msg{size(in_msg,1)+1,1} = FN.inbound_msg{j};
                               fm_id = [fm_id; FN.from_id(j)];
                           end % for j = ...
                           % execute factor function
                           snd_msg = FN.factor_fun(in_msg, fm_id, FN.from_id(i));
                           % send message
                           FN.from_node{i}.rx_msg(FN, snd_msg);
                           % update outbound record
                           k = find(FN.to_id == FN.from_id(i));
                           FN.to_node{k} = FN.from_node{i};
                           FN.outbound_msg{k} = snd_msg;
                       end % for i = ...
                   otherwise
               end % switch
           end % if loopy
       end % function rx_msg
       
   end % methods
   
   methods (Abstract = true)
       msg = factor_fun(FN, in_msg, from_id, to_id)
   end
end 
