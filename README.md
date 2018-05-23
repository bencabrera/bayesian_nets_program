# Updating Probabilistic Knowledge on Condition/Event Nets using Bayesian Networks

This repository contains the implementation and experimental results for the article *"Updating Probabilistic Knowledge on Condition/Event Nets using Bayesian Networks"* submitted to *Concur '18*. 

## Abstract 
The paper extends Bayesian networks (BNs) by a mechanism for dynamic changes to the probability distributions represented by BNs. One application scenario is the process of knowledge acquisition of an observer interacting with a system. In particular, the paper considers condition/event nets where the observer’s knowledge about the current marking is a probability distribution over markings. The observer can interact with the net to deduce information about the marking by requesting certain transitions to fire and observing their success or failure. Aiming for an efficient implementation of dynamic changes to probability distributions of BNs, we consider a slightly more general class of networks that form the arrows of a free PROP with a commutative comonoid structure, also known as term graphs. The algebraic structure of such PROPs supplies us with a compositional semantics that functorially maps BNs to their underlying probability distribution and, in particular, it provides a convenient means to describe structural updates of networks.

## Program
The programs in this repository implement CNUs and GBNs as described in the paper. Additionally, we implemented the simplification steps for GBNs able turn every GBN into an OBN again, reducing the complexity of evaluating it. In order to have a reference for comparing runtimes we also implemented a variant of CNUs using a simple joint distribution as the underlying model to store probabilities.
Mostly for development purposes we also implemented functions to load, save and draw GBNs.

### Simplification of GBNs
The following GIF is meant to give an impression of how the simplification of an arbitrary GBN works. We use the local simplifications (F1)-(F5), as well as eliminating nodes without connection to output ports and switching substochastic matrices to the front and renormalizing.

The GBN is visualized using GraphViz.
Vertices visualized as small dots and labelled with `i_...` or `o_...` symbolize inputs and outputs of the GBN. Edge are labeled with tuples representing the ports of source and target vertex the edge is connected to.

![Simplification GIF](images/simplification.gif)

### CNU Operations modelled by a GBN

### Runtime Results

![Comparison CNU with Joint Distribution or GBN](images/joint_dist_gbn_comparison.png) ![GBN Behaviour with increasing number of places](images/bin_gbn_comparison.png)

## Installation
