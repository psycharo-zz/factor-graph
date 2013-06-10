# Global TODO
* Learn univariate gaussians: DONE
* Create a separate branch in the factor-graph repo
* Learn univariate mixture models: IN PROGRESS

* Algonquin implementation: based on univariate mixtures

* Learn multivariate gaussians: NOT NECESSARY?
* Learn multivariate mixture models: NOT NECESSARY?
* Check how current implementation of mixtures correspond to the gates [Minka'200x]
* Check how the current implementation correspond to the message passing on FACTOR GRAPHS
  * ?? the difference is that in factor graphs factors can be NOT ONLY DISTRIBUTIONS
       but also deterministic functions


* Automatical inference code generation
* Matlab Bindings (priority)

* Tests for messages (depending on current messages from parents|observed / non-observed)




# Current TODO

* Not clear whether the message should be represented as an array or
  a distribution.
  * probably the distribution is better - since the child should "know"
    which parameters are there

* Not clear whether the messages should be passed by the network itself or locally
  * Having a global table of messages, stored in `class Network` should speed things up


* Every node should implement following methods:
  1. `Moments messageToChildren()`
     * computes expectation of the natural statistics
     * in case the node is OBSERVED, then simply the values themselves
       are incorporated.

  2. `Parameters messageToParent(parent)`
     * computes the parameter vector of the distribution
       of the same form as the parent.
     * e.g. for a gaussian it should be gaussian parameter vector for mean prior,
       and the gamma/wishart for the precision prior

  3. `updatePosterior()`
     * computes
     * there should be a submethod that computes the part based on parents,
       which will depend on the type of the parents that the node has: e.g.
       if they are constants there is no need to retrieve messages from the
       parent, but in any case there should be a locally saved `Moments` from
       each parent
     * as for parents,


  4. `double logEvidenceLowerBound()`
     * computes the part of the lower bound on the log-evidence that the
       node constitutes

     * in fact, this function relies on two other methods:
     `double logEvidenceLowerBoundObserved()` and `double logEvidenceLoweboundHidden()`
     that compute the lower bound on the log-evidence if the variable is observed and
     hidden correspondingly. Note that this means that we actually model the network
     as a bayesian network instead of a factor graph.

  5. `double logNormalization()`
     * computes the 'g()' function of the distribution
     * the problem is that for constant parents it is not clear how it should be computed
       (since the constant parents do not provide messages). one solution is to create a
       ConstantGaussian distribution that provides constants as the output and does not
       update its parameters.




# Questions

* What are arrays? Should there be a "special" variable, or for now just
  a simple approach with a vector of variables will work?
* The same question for mixtures.

* What to do with schedules? In which order the messages should be passed?

* What are messages here really? And what kind of information a node should
  "know" about messages it receives?
  * Messages are distribution parameters.

  * When sent from parents to children, they are expectations of natural
    statistics vector w.r.t. current natural parameter vector. The form of the
    natural statistics vector depends only on the form of the parent
    distribution, and the child should "know" how to compute the part of
    its udated posterior from the input vector.

  * When sent from children to parents, they are reparameterized natural
    parameter vectors. The reparameterization is made such that the form
    is the same as the parent.

  * After getting all the messages from the neighbors, the approx. posterior
    can be updated, simply by summing up the natural param. vector from parents
    _and_ natural statistics vectors


  * ??? Some nodes that do not have parents-variables (meaning that the paremeters
    of their distributions are fixed), have _fixed_ messages from parents ???



* everything is defined as a set of variables connected via factors
* every variable can be:
    -continuous/discrete
    -observed (point mass) / unobserved (the actual distribution)

